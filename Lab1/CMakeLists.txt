cmake_minimum_required(VERSION 3.28)
project(DSP)

set(CMAKE_CXX_STANDARD 20)
find_package(ALSA REQUIRED)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCE_FILES
    "src/*"
)

add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
        include/track.h
        src/track.cpp
        include/notes.h
        include/melody.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE ALSA::ALSA)


