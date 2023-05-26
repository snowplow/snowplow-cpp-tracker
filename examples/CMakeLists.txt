cmake_minimum_required(VERSION 3.15)
project(snowplow-example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

if(NOT TARGET snowplow::snowplow)
    find_package(snowplow REQUIRED CONFIG)
endif()

add_executable(snowplow-example main.cpp)
target_link_libraries(snowplow-example PRIVATE snowplow::snowplow)
