set(REMOTE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the remote directory")

include_directories(
    ${ESF_BASE_DIR}
)

file(GLOB_RECURSE REMOTE_SOURCES
    "${REMOTE_DIR}/*.cpp"
)

add_library(remote SHARED ${REMOTE_SOURCES})
target_include_directories(remote PUBLIC ${REMOTE_DIR} connectivity)
target_link_libraries(remote PUBLIC connectivity)