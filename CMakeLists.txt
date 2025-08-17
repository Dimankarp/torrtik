cmake_minimum_required(VERSION 3.27)

project(Torrtik LANGUAGES CXX VERSION 0.0.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan Dependencies
find_package(Boost CONFIG REQUIRED COMPONENTS asio beast unit_test_framework system)

# System Dependencies
# Maybe update conan to include it separately as system req
find_package(OpenSSL REQUIRED)

# Config files
configure_file(config/trrtconfig.h.in trrtconfig.h)

# Options
option(TRRT_ENABLE_LOGGING "Enable torrtik library logging" OFF)
if(TRRT_ENABLE_LOGGING)
    add_compile_definitions(TRRT_ENABLE_LOGGING)
endif()

# Getting sources & includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${PROJECT_BINARY_DIR}
)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp")


add_library(torrtik_lib STATIC
    ${LIB_SOURCES}
)
target_link_libraries(torrtik_lib PRIVATE  PUBLIC OpenSSL::Crypto  Boost::system)


# Create executable
add_executable(torrtik
    ${APP_SOURCES}
)
target_link_libraries(torrtik PRIVATE torrtik_lib)


# Testing
enable_testing()
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
foreach(TEST_FILE IN LISTS TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_FILE}" NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE Boost::unit_test_framework torrtik_lib)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()