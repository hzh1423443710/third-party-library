cmake_minimum_required(VERSION 3.20)

project(test_xml)

# sudo pacman -S pugixml
find_package(PugiXML CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pugixml)