
add_executable(duckdb-app main.cc)

target_compile_features(duckdb-app PRIVATE cxx_std_23)

target_link_libraries(duckdb-app
    Boost::program_options
    duckdb  # Use DuckDB from CMake config if possible and then review need of duckdb package in runtime
)

install(TARGETS duckdb-app RUNTIME DESTINATION bin)
