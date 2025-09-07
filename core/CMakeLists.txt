set(ESF_CORE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the core directory")

file(GLOB_RECURSE ESF_CORE_SOURCES
    "${ESF_CORE_DIR}/*.cpp"
)

add_library(esf_core SHARED ${ESF_CORE_SOURCES})
target_include_directories(esf_core PUBLIC ${ESF_CORE_DIR})