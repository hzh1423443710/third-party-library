cmake_minimum_required(VERSION 3.20)

project(test_json)

# https://github.com/nlohmann/json
# sudo pacman -S nlohmann-json
find_package(nlohmann_json REQUIRED)

add_executable(test_json main.cpp)
target_link_libraries(test_json PRIVATE nlohmann_json::nlohmann_json)