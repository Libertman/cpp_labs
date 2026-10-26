cmake_minimum_required(VERSION 3.16)
project(Calculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Директории вывода бинарных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins")

# Папка с плагинами для рантайма
set(PLUGINS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/plugins")
file(MAKE_DIRECTORY "${PLUGINS_OUTPUT_DIR}")

# Базовая библиотека калькулятора (ядро)
set(CORE_SOURCES
    src/DynamicLibrary.cpp
    src/Plugin.cpp
    src/PluginManager.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Calculator.cpp
)

add_library(calc_core STATIC ${CORE_SOURCES})

target_include_directories(calc_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Для dlopen/dlsym на Linux/POSIX
if(UNIX AND NOT APPLE)
    target_link_libraries(calc_core PUBLIC ${CMAKE_DL_LIBS})
endif()

# Исполняемый файл калькулятора (calc.exe на Windows, calc на POSIX)
add_executable(calc src/main.cpp)
set_target_properties(calc PROPERTIES OUTPUT_NAME "calc")
target_link_libraries(calc PRIVATE calc_core)

# Макрос для создания плагинов
function(add_calc_plugin TARGET_NAME SOURCE_FILE)
    add_library(${TARGET_NAME} SHARED ${SOURCE_FILE})
    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    
    # Настройки вывода плагина в папку plugins
    set_target_properties(${TARGET_NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME "${TARGET_NAME}"
        LIBRARY_OUTPUT_DIRECTORY "${PLUGINS_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PLUGINS_OUTPUT_DIR}"
    )

    if(UNIX AND NOT APPLE)
        target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_DL_LIBS})
    endif()
    
    # Гарантируем, что плагины собираются до запуска калькулятора
    add_dependencies(calc ${TARGET_NAME})
endfunction()

# Добавление плагинов согласно ТЗ
add_calc_plugin(funcsin plugins/sin/sin_plugin.cpp)
add_calc_plugin(funcdeg plugins/deg/deg_plugin.cpp)
add_calc_plugin(funcln plugins/ln/ln_plugin.cpp)
add_calc_plugin(funccos plugins/cos/cos_plugin.cpp)
add_calc_plugin(funcsqrt plugins/sqrt/sqrt_plugin.cpp)
add_calc_plugin(invalid_dummy plugins/invalid_dummy/invalid_dummy.cpp)
