cmake_minimum_required(VERSION 3.15)
project(snowplow-install-tester-client)

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED YES)

find_package(snowplow REQUIRED CONFIG)

add_executable(install-test main.cpp)
target_link_libraries(install-test PRIVATE snowplow::snowplow)
