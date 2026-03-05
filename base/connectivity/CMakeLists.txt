set(CONNECTIVITY_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the connectivity directory")

include_directories(
    ${ESF_BASE_DIR}
)

file(GLOB_RECURSE CONNECTIVITY_SOURCES
    "${CONNECTIVITY_DIR}/*.cpp"
)

add_library(connectivity SHARED ${CONNECTIVITY_SOURCES})
target_include_directories(connectivity PUBLIC ${CONNECTIVITY_DIR} ${ESF_CORE_DIR} ${ESF_FREERTOS_DIR} stm32cubemx)
target_link_libraries(connectivity PUBLIC stm32cubemx esf_freertos)