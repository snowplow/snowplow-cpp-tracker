cmake_minimum_required(VERSION 3.15) # 3.15 for CMAKE_FIND_PACKAGE_PREFER_CONFIG and CMAKE_MSVC_RUNTIME_LIBRARY

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

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

SET(SNOWPLOW_SOURCES
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

IF (APPLE)
   SET(SNOWPLOW_SOURCES ${SNOWPLOW_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/detail/utils/utils_macos.mm
        ${CMAKE_CURRENT_SOURCE_DIR}/include/snowplow/detail/http/request_macos.mm)
ENDIF()

add_library(snowplow ${SNOWPLOW_SOURCES})
add_library(snowplow::snowplow ALIAS snowplow)

target_include_directories(snowplow
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)

set_target_properties(snowplow PROPERTIES
    VERSION ${SNOWPLOW_TRACKER_VERSION})

# add nlohmann/json library
include(FetchContent)
set(NLOHMANN_JSON_VERSION 3.10.5)
if(SNOWPLOW_USE_EXTERNAL_JSON)
    find_package(nlohmann_json ${NLOHMANN_JSON_VERSION} REQUIRED)
else()
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG        v${NLOHMANN_JSON_VERSION}
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()
target_link_libraries(snowplow PUBLIC nlohmann_json::nlohmann_json)

# add sqlite3
if(SNOWPLOW_USE_EXTERNAL_SQLITE)
    find_package(SQLite3 REQUIRED)
    target_link_libraries(snowplow PRIVATE SQLite::SQLite3)
else()
    FetchContent_Declare(sqlite_amalgamation URL "https://www.sqlite.org/2022/sqlite-amalgamation-3380500.zip")
    if (NOT sqlite_amalgamation_POPULATED)
        FetchContent_Populate(sqlite_amalgamation)
    endif()
    target_include_directories(snowplow PRIVATE ${sqlite_amalgamation_SOURCE_DIR})
    target_sources(snowplow PRIVATE ${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c)
endif()

find_package(Threads)
target_link_libraries(snowplow PRIVATE Threads::Threads)

if (APPLE)
    target_link_libraries(snowplow PRIVATE
        "-framework CoreFoundation"
        "-framework Foundation"
        "-framework CoreServices")
endif()

if (NOT APPLE AND NOT WIN32)
    find_package(CURL REQUIRED)
    target_link_libraries(snowplow PRIVATE CURL::libcurl)
    set(SNOWPLOW_NEEDS_CURL 1)
    target_link_libraries(snowplow PRIVATE ${CMAKE_DL_LIBS})
    find_package(LibUUID REQUIRED)
    target_link_libraries(snowplow PRIVATE libuuid::libuuid)
    set(SNOWPLOW_NEEDS_LIBUUID 1)
endif ()

include(CMakePackageConfigHelpers)

set(create_install_target ON)

if(NOT SNOWPLOW_USE_EXTERNAL_JSON)
    set(create_install_target OFF)
    message(STATUS "Install target not added because SNOWPLOW_USE_EXTERNAL_JSON is OFF")
endif()

if(NOT BUILD_SHARED_LIBS AND NOT SNOWPLOW_USE_EXTERNAL_SQLITE)
    set(create_install_target OFF)
    message(STATUS "Install target not added because SNOWPLOW_USE_EXTERNAL_SQLITE is OFF while building a static library (BUILD_SHARED_LIBS is OFF)")
endif()

if(create_install_target)
    install(TARGETS snowplow EXPORT snowplow-export
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)

    install(DIRECTORY include/snowplow
            DESTINATION include
            FILES_MATCHING
                PATTERN *.hpp
                PATTERN *.h
                PATTERN sqlite3.hpp EXCLUDE)

    install(EXPORT snowplow-export
      DESTINATION lib/cmake/snowplow
      NAMESPACE snowplow::
      FILE snowplow-targets.cmake)

    configure_file(
        cmake/snowplow-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/configure_file/snowplow-config.cmake
        @ONLY)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/configure_file/snowplow-config-version.cmake
        COMPATIBILITY SameMajorVersion)
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/configure_file/snowplow-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/configure_file/snowplow-config-version.cmake
        DESTINATION lib/cmake/snowplow)
    if(SNOWPLOW_NEEDS_LIBUUID)
        install(FILES ${SNOWPLOW_CMAKE_DIR}/FindLibUUID.cmake
            DESTINATION lib/cmake/snowplow)
    endif()
endif()

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
        ${CMAKE_CURRENT_SOURCE_DIR}/test/snowplow_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/integration/integration_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/integration/micro.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/configuration/network_configuration_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/configuration/emitter_configuration_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/utils_test.cpp)

    if (NOT WIN32)
        find_package(CURL REQUIRED)
        target_link_libraries(snowplow-tests CURL::libcurl)
    endif ()

    target_link_libraries(snowplow-tests snowplow)
endif()

if(SNOWPLOW_BUILD_EXAMPLE)
    add_subdirectory(examples)
endif()

if(SNOWPLOW_BUILD_PERFORMANCE)
    add_executable(snowplow-performance
        ${CMAKE_CURRENT_SOURCE_DIR}/performance/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/performance/run.cpp)

    target_link_libraries(snowplow-performance snowplow)
endif()
