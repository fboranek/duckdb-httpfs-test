#include <boost/program_options.hpp>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <iostream>
#include <thread>

namespace {


struct AppConfig {
    std::string s3Host{};
    std::string s3AccessKey{};
    std::string s3SecretKey{};
    std::string s3Bucket{};
};

class App {
public:
    App(const AppConfig& config)
        : config{config}
    {}

    void testSelect()
    {
        std::cout << "testSelect: Starting DuckDB initialization\n";
        auto db  = duckdb::DuckDB{nullptr};
        auto con = duckdb::Connection{db};

        auto check = [](auto&& x) -> decltype(auto) {
            if (!x) throw std::runtime_error{"DuckDB: Connection::Query returned nullptr!"};
            if (x->HasError()) throw std::runtime_error{std::format("DuckDB: {}", x->GetError())};
            return std::forward<decltype(x)>(x);
        };

        std::cout << "testSelect: Creating S3 secrets\n";
        check(con.Query(
            R"sql(
            CREATE SECRET test_s3 (
                TYPE s3,
                KEY_ID $1,
                SECRET $2,
                ENDPOINT $3,
                REGION 's3',
                URL_STYLE 'path',
                VERIFY_SSL false,
                SCOPE $4::VARCHAR
            );
            )sql",
            config.s3AccessKey,
            config.s3SecretKey,
            config.s3Host,
            "s3://" + config.s3Bucket));

        std::cout << "testSelect: Listing S3 files in s3://" << config.s3Bucket
                  << " using httpfs extension\n";
        auto files = check(con.Query(
            R"sql(
            SELECT file FROM glob('s3://' || $1 || '/*.parquet');
            )sql",
            config.s3Bucket));


        std::cout << "testSelect: Files in bucket:\n";
        std::int64_t filesCount = 0;
        for (auto&& row : *files) {
            ++filesCount;
            std::cout << row.GetValue<std::string>(0) << "\n";
        }
        std::cout << "testSelect: Total files " << filesCount << "\n";
    }

protected:
    AppConfig config;
};

template<typename T>
T getConfValue(const char* name)
{
    auto value = std::getenv(name);
    if (!value) {
        throw std::runtime_error{std::format("Missing environment variable {}", name)};
    }
    return T{value};
}

} // namespace

int main(int argc, char* argv[])
try {
    namespace po = boost::program_options;

    po::options_description desc("Usage");
    desc.add_options()("help,h", "Produce help message.");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

    if (vm.count("help")) {
        std::cerr << "Test App to demonstrate using DuckDB + httpfs in upcoming Debian Forky. "
                  << "The goal is to load the httpfs external extension from pre-built dynamic "
                     "libraries.\n\n"
                  << "All configuration is read from environment variables"
                  << "\n";
        std::cerr << desc << "\n";
        return EXIT_FAILURE;
    }
    po::notify(vm); // throw if required options are missing

    const auto config = AppConfig{
        .s3Host      = getConfValue<std::string>("MY_APP_S3_HOST"),
        .s3AccessKey = getConfValue<std::string>("MY_APP_S3_ACCESS_KEY"),
        .s3SecretKey = getConfValue<std::string>("MY_APP_S3_SECRET_KEY"),
        .s3Bucket    = getConfValue<std::string>("MY_APP_S3_BUCKET"),
    };

    if (config.s3Host.empty()) {
        throw std::runtime_error{"S3 host is empty"};
    }

    App app{config};
    app.testSelect();
    std::cout << "Success.\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
