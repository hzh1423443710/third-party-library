cmake_minimum_required(VERSION 3.20)

# 项目名和版本号
project(test_redis)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加源文件
add_executable(test_redis main.cpp)

# 查找 hiredis 库
find_library(HIREDIS_LIB NAMES hiredis)
message(STATUS "hiredis lib: ${HIREDIS_LIB}")

# 链接 hiredis 库     g++ main.cpp -lhiredis
target_link_libraries(test_redis PRIVATE ${HIREDIS_LIB})
