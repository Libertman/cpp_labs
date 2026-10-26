cmake_minimum_required(VERSION 3.16)
project(GEMS VERSION 1.0 LANGUAGES CXX)

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
    main.cpp
    Board.cpp
    Game.cpp
    Gem.cpp
)

add_executable(GEMS ${SOURCES})

target_link_libraries(GEMS PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(GEMS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    add_custom_command(TARGET GEMS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/Debug"
            $<TARGET_FILE_DIR:GEMS>
    )
endif()
