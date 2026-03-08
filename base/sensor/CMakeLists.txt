set(SENSOR_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the sensor directory")

include_directories(
    ${ESF_BASE_DIR}
)

file(GLOB_RECURSE SENSOR_SOURCES
    "${SENSOR_DIR}/*.cpp"
)

add_library(sensor SHARED ${SENSOR_SOURCES})
target_include_directories(sensor PUBLIC ${SENSOR_DIR} connectivity)
target_link_libraries(sensor PUBLIC connectivity)