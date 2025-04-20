cmake_minimum_required(VERSION 3.20)

project(jwt-test)

# Add the directory where jwt-cpp is installed to the prefix path
list(APPEND CMAKE_PREFIX_PATH "/usr/lib")

# 或 (jwt-cpp的cmake文件在 /usr/lib/cmake/ 下)
# set(jwt-cpp_DIR "/usr/lib/cmake/")
find_package(jwt-cpp CONFIG REQUIRED)

add_executable(jwt-test main.cpp)
target_link_libraries(jwt-test PRIVATE
    jwt-cpp::jwt-cpp
)