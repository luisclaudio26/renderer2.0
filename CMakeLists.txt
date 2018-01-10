cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(SET CMP0015 NEW)
project(Raytracer)

#TODO:make less use of hardcoded GCC flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -std=c++11 -g")

#Headers
include_directories(include)

#Source files
file(GLOB SOURCES "src/*.cpp"
                  "src/core/*.cpp"
                  "src/camera/*.cpp"
                  "src/util/*.cpp"
                  "src/shape/*.cpp"
                  "src/texture/*.cpp")

#Link static libraries
link_directories(3rdparty)

add_executable(raytracer ${SOURCES})
target_link_libraries(raytracer tinyobjloader)
