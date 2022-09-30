cmake_minimum_required(VERSION 3.5)

# Definitions
set(PROJECT_NAME pikuma-3d)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

project(${PROJECT_NAME})
IF (WIN32)
    set(SDL2_DIR C:/SDL2/cmake)
ENDIF()

find_package(SDL2 REQUIRED)

# Include paths
set(INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
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

file(GLOB_RECURSE SOURCE_FILES src/*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
IF (WIN32)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_LIB ${SDL2_LIBRARIES})
ELSE()
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_LIB ${SDL2_LIBRARIES} m)
ENDIF()

# Copy assets
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
