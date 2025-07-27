cmake_minimum_required(VERSION 3.27)

project(Torrtik CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Dependencies
find_package(Boost CONFIG REQUIRED COMPONENTS asio beast unit_test_framework system)


# Getting sources & includes

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/app
)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp")


add_library(torrtik_lib STATIC
    ${LIB_SOURCES}
)


# Create executable
add_executable(torrtik
    ${APP_SOURCES}
)
target_link_libraries(torrtik PRIVATE Boost::system torrtik_lib)


# Testing
enable_testing()
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
foreach(TEST_FILE IN LISTS TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_FILE}" NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE Boost::unit_test_framework torrtik_lib)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()