cmake_minimum_required(VERSION 3.28)
project(DSP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Build system: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Build system version: ${CMAKE_SYSTEM_VERSION}")

set(INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME}
        src/main.cpp
)

add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../Plugins
        ${CMAKE_CURRENT_BINARY_DIR}/Plugins/build
)

target_link_libraries(${PROJECT_NAME} PRIVATE Plugins)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()