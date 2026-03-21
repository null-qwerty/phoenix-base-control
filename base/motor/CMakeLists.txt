set(MOTOR_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the motor directory")

include_directories(
    ${ESF_BASE_DIR}
)

file(GLOB_RECURSE MOTOR_SOURCES
    "${MOTOR_DIR}/*.cpp"
)

add_library(motor STATIC ${MOTOR_SOURCES})
target_include_directories(motor PUBLIC ${MOTOR_DIR} connectivity)
target_link_libraries(motor PUBLIC connectivity)