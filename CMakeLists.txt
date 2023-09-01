cmake_minimum_required(VERSION 3.15)
project(chess_engine CXX)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

file(GLOB chess_src
    "src/*.cpp"
)

add_executable(chess ${chess_src})
target_link_libraries(chess ${wxWidgets_LIBRARIES})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
