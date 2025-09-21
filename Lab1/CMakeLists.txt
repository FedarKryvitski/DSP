cmake_minimum_required(VERSION 3.28)
project(DSP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

MESSAGE(STATUS "Build system: ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "Build system version: ${CMAKE_SYSTEM_VERSION}")

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCE_FILES
    "src/*"
)

add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building for Linux - ALSA support enabled")
    find_package(ALSA REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ALSA::ALSA)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LINUX_BUILD)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Building for Windows - ALSA support disabled")
    target_compile_definitions(${PROJECT_NAME} PRIVATE WINDOWS_BUILD)

else()
    message(WARNING "Building for unknown system: ${CMAKE_SYSTEM_NAME}")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})