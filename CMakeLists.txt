cmake_minimum_required(VERSION 3.25)

project(Embedded-System-Framework LANGUAGES C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ESF_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the ESF root directory")

add_subdirectory(core)
add_subdirectory(FreeRTOS)
add_subdirectory(base)

add_library(esf INTERFACE)
target_link_libraries(esf INTERFACE esf_core esf_freertos)
target_include_directories(esf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/core)