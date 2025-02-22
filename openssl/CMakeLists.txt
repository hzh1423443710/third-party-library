cmake_minimum_required(VERSION 3.20)

project(test_openssl)

find_package(OpenSSL REQUIRED)

add_executable(test_encrypt test_encrypt.cpp)
target_link_libraries(test_encrypt PRIVATE OpenSSL::SSL OpenSSL::Crypto)