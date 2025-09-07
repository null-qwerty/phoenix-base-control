cmake_minimum_required(VERSION 3.25)

project(Embedded-System-Framework LANGUAGES C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(core)

add_library(esf INTERFACE)
target_link_libraries(esf INTERFACE esf_core)
target_include_directories(esf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/core)