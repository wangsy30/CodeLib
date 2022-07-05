cmake_minimum_required(VERSION 3.1)
file(GLOB_RECURSE sources *.hpp *.cpp *.h *.cu)
#enable_language(CUDA)
set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)

set(CMAKE_CXX_COMPILIER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-write-strings")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,$ORIGIN")

include_directories(/usr/local/cuda/include)
link_directories(/usr/local/cuda/lib64)
find_package(CUDA REQUIRED)

find_package(OpenCV REQUIRED)


cuda_add_library(detector SHARED ${sources})
target_link_libraries(detector nvinfer nvinfer_plugin nvcaffe_parser "stdc++fs")
