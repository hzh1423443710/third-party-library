
cmake_minimum_required(VERSION 3.20)

# 项目名和版本号
project(test_mysql)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_MYSQL_CONNECTOR_INLCUDE /usr/local/mysql/connector-c++-8.0.33/include)

# 查找 MySQL Connector C++ 库
find_library(LIB_MYSQL_CONNECTOR_CPP NAMES mysqlcppconn PATHS /usr/local/mysql/connector-c++-8.0.33/lib64/debug REQUIRED)
message(STATUS "mysqlcppconn: ${LIB_MYSQL_CONNECTOR_CPP}")

add_executable(test_mysql mysql++.cpp)
target_include_directories(test_mysql PRIVATE ${LIB_MYSQL_CONNECTOR_INLCUDE})
target_link_libraries(test_mysql ${LIB_MYSQL_CONNECTOR_CPP})
