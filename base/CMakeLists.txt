set(ESF_BASE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the base directory")

include_directories(
    ${ESF_ROOT_DIR}
)

add_subdirectory(connectivity)

add_library(esf_base INTERFACE)
target_link_libraries(esf_base INTERFACE connectivity)