cmake_minimum_required(VERSION 3.20)

project(game_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)

add_executable(game_engine
    src/main.cpp
    src/window.cpp
    # src/framebuffer.cpp
    # 後で増やす
)

target_include_directories(game_engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game_engine
    PRIVATE
        Eigen3::Eigen
        SDL3::SDL3
)

install(
    TARGETS game_engine
    DESTINATION .
)
