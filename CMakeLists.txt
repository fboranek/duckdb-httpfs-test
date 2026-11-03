cmake_minimum_required(VERSION 3.25)

project(duckdb-app VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Boost CONFIG REQUIRED COMPONENTS
    program_options
)
# find_package(DuckDB CONFIG REQUIRED)

add_subdirectory(src)
enable_testing()
