set(ESF_FREERTOS_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the FreeRTOS directory")

include_directories(
    ${ESF_ROOT_DIR}
)

file(GLOB_RECURSE ESF_FREERTOS_SOURCES
    "${ESF_FREERTOS_DIR}/*.cpp"
)

add_library(esf_freertos SHARED ${ESF_FREERTOS_SOURCES})
target_include_directories(esf_freertos PUBLIC ${ESF_FREERTOS_DIR} ${MX_Include_Dirs})
target_link_libraries(esf_freertos PUBLIC FreeRTOS)