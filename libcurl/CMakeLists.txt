cmake_minimum_required(VERSION 3.20)

project(libcurl-study)

add_executable(libcurl-smtp smtp.cpp)

target_link_libraries(libcurl-smtp PRIVATE curl)
