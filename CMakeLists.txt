cmake_minimum_required(VERSION 3.14)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/constants.hpp CONSTANTS_HPP_CONTENTS)
string(REGEX MATCH "const string SNOWPLOW_TRACKER_VERSION_LABEL = \"cpp-[^\"]+\"" SNOWPLOW_TRACKER_VERSION ${CONSTANTS_HPP_CONTENTS})
string(REGEX REPLACE "const string SNOWPLOW_TRACKER_VERSION_LABEL = \"cpp-([^\"]+)\"" "\\1" SNOWPLOW_TRACKER_VERSION ${SNOWPLOW_TRACKER_VERSION})
message(STATUS "Tracker version: ${SNOWPLOW_TRACKER_VERSION}")

project(snowplow VERSION ${SNOWPLOW_TRACKER_VERSION} LANGUAGES CXX C)

set(SNOWPLOW_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake" CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SNOWPLOW_CMAKE_DIR}")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS  "Create a module definition (.def) on Windows." ON)
option(SNOWPLOW_BUILD_TESTS "Build test program" OFF)
option(SNOWPLOW_BUILD_EXAMPLE "Build example program" OFF)
option(SNOWPLOW_BUILD_PERFORMANCE "Build performance testing program" OFF)
option(SNOWPLOW_USE_EXTERNAL_JSON "Use an external JSON library" OFF)
option(SNOWPLOW_USE_EXTERNAL_SQLITE "Use an external SQLite library" OFF)

add_library(snowplow
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/detail/base64/base64.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/client_session.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/cracked_url.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/emitter/emitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/emitter/retry_delay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/http/http_client_windows.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/http/http_client_apple.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/http/http_client_curl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/http/http_request_result.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/payload/payload.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/payload/event_payload.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/payload/self_describing_json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/events/event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/events/screen_view_event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/events/self_describing_event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/events/structured_event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/events/timing_event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/storage/sqlite_storage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/subject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/tracker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/detail/utils/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/configuration/emitter_configuration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/configuration/network_configuration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/configuration/session_configuration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/snowplow.cpp)

target_include_directories(snowplow
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(snowplow PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# add nlohmann/json library
include(FetchContent)
if(SNOWPLOW_USE_EXTERNAL_JSON)
    find_package(nlohmann_json 3.10.5 REQUIRED)
else()
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG        v3.10.5
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()
target_link_libraries(snowplow nlohmann_json::nlohmann_json)

# add sqlite3
if(SNOWPLOW_USE_EXTERNAL_SQLITE)
    find_package(SQLite3 REQUIRED)
    target_include_directories(snowplow PUBLIC ${SQLite3_INCLUDE_DIRS})
    target_link_libraries(snowplow ${SQLite3_LIBRARIES})
else()
    FetchContent_Declare(sqlite_amalgamation URL "https://www.sqlite.org/2022/sqlite-amalgamation-3380500.zip")
    if (NOT sqlite_amalgamation_POPULATED)
        FetchContent_Populate(sqlite_amalgamation)
    endif()
    target_include_directories(snowplow PUBLIC ${sqlite_amalgamation_SOURCE_DIR})
    target_sources(snowplow PUBLIC ${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c)
endif()

find_package(Threads)
target_link_libraries(snowplow Threads::Threads)

if (APPLE)
    target_sources(snowplow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/detail/utils/utils_macos.mm)
    find_library(COREFOUNDATION CoreFoundation)
    target_link_libraries(snowplow ${COREFOUNDATION})
    find_library(CFNETWORK CFNetwork)
    target_link_libraries(snowplow ${CFNETWORK})
    find_library(FOUNDATION Foundation)
    target_link_libraries(snowplow ${FOUNDATION})
    find_library(CORESERVICES CoreServices)
    target_link_libraries(snowplow ${CORESERVICES})
endif()

if (NOT WIN32)
    find_package(CURL REQUIRED)
    target_include_directories(snowplow PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(snowplow ${CURL_LIBRARIES})
    target_link_libraries(snowplow ${CMAKE_DL_LIBS})
endif()

if (NOT APPLE AND NOT WIN32)
    find_package(UUID REQUIRED)
    include_directories("${UUID_INCLUDE_DIR}")
    target_link_libraries(snowplow "${UUID_LIBRARY}")
endif ()

target_include_directories(snowplow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(SNOWPLOW_BUILD_TESTS)
  add_executable(snowplow-tests
    ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/http/test_http_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/client_session_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/cracked_url_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/emitter/emitter_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/emitter/retry_delay_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/http/http_client_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/http/http_request_result_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/payload/payload_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/payload/event_payload_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/payload/self_describing_json_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/storage/sqlite_storage_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/subject_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/tracker_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/utils_test.cpp)

    target_link_libraries(snowplow-tests snowplow)
    set_target_properties(snowplow-tests PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
endif()

if(SNOWPLOW_BUILD_EXAMPLE)
    add_executable(snowplow-example
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/main.cpp)

    target_link_libraries(snowplow-example snowplow)
    set_target_properties(snowplow-example PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
endif()

if(SNOWPLOW_BUILD_PERFORMANCE)
    add_executable(snowplow-performance
        ${CMAKE_CURRENT_SOURCE_DIR}/performance/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/performance/run.cpp)

    target_link_libraries(snowplow-performance snowplow)
    set_target_properties(snowplow-performance PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
endif()
