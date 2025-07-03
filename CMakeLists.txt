# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MultithreadedCompressor)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)

include_directories(include)
add_executable(MultithreadedCompressor src/main.cpp src/Compressor.cpp)
target_link_libraries(MultithreadedCompressor ZLIB::ZLIB)
