cmake_minimum_required(VERSION 3.16)
project(ARCANOID VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOCS OFF CACHE BOOL "" FORCE)

set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(SFML)

set(SOURCES
    Ball.cpp
    Game.cpp
    Paddle.cpp
    main.cpp
)

add_executable(ARCANOID ${SOURCES})

target_link_libraries(ARCANOID PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(ARCANOID PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    add_custom_command(TARGET ARCANOID POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/Debug"
            $<TARGET_FILE_DIR:ARCANOID>
    )
endif()
