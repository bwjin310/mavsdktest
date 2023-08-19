cmake_minimum_required(VERSION 3.5)

project(mavsdk_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mavsdk_test main.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(mavsdk_test ${PROJECT_SOURCE_DIR}/lib/libmavsdk.so)
