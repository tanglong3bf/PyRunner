cmake_minimum_required(VERSION 3.0)

project(testPyRunner)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} test.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE "/usr/include/python3.8")

target_link_libraries(${PROJECT_NAME} PRIVATE python3.8)

file(COPY testAdd.py DESTINATION ${CMAKE_BINARY_DIR})
file(COPY scripts DESTINATION ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
