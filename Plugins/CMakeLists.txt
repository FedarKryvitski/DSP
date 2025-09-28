cmake_minimum_required(VERSION 3.28)
project(Plugins VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Plugins build system: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Plugins system version: ${CMAKE_SYSTEM_VERSION}")

add_library(${PROJECT_NAME} STATIC
        src/wav_file.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building for Linux - ALSA support enabled")

    find_package(ALSA REQUIRED)
    target_sources(${PROJECT_NAME} PRIVATE
            src/audio_player.cpp
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE ALSA::ALSA)
else()
    message(STATUS "For system ${CMAKE_SYSTEM_NAME} ALSA support disabled")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)