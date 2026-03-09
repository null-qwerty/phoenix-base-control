set(ESF_CORE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the core directory")

include_directories(
    ${ESF_ROOT_DIR}
)

file(GLOB_RECURSE ESF_CORE_SOURCES
    "${ESF_CORE_DIR}/*.hpp"
    "${ESF_CORE_DIR}/*.cpp"
)

add_library(esf_core STATIC ${ESF_CORE_SOURCES})
target_include_directories(esf_core PUBLIC ${ESF_CORE_DIR})