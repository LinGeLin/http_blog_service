cmake_minimum_required(VERSION 3.10)

project (blog)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(service service.cpp)

target_link_libraries(service pthread mysqlpp)