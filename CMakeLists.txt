cmake_minimum_required(VERSION 3.20)
project(Chip8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hello
    main.cpp
    chip8.cpp       
)

target_include_directories(hello PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hello PRIVATE SDL3)