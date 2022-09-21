cmake_minimum_required(VERSION 3.5)

# Definitions
set(PROJECT_NAME pikuma-3d)

project(${PROJECT_NAME})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lm")

# Include paths
set(INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/external/SDL
    ${PROJECT_SOURCE_DIR}/include
)

# Assign the include directories
include_directories(${INCLUDE_DIRS})

# Find library sources
file(GLOB_RECURSE LIB_SRC_FILES include/*/*.c)

# Build static/shared library
add_library(${PROJECT_NAME}_LIB STATIC ${LIB_SRC_FILES})

# Install library
install(TARGETS ${PROJECT_NAME}_LIB DESTINATION lib)

# Install library headers
file(GLOB_RECURSE LIB_INC_FILES include/*/*.h)
install(FILES ${LIB_INC_FILES} DESTINATION include)

# SDL 2
add_subdirectory(external/SDL)

file(GLOB_RECURSE SOURCE_FILES src/*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_LIB SDL2-static)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
