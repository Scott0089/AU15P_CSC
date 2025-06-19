set(PROJECT_NAME AU15P_CSC2)
cmake_minimum_required(VERSION 3.10)

# Project information
project(${PROJECT_NAME}
    VERSION 1.0.0
    DESCRIPTION "${PROJECT_NAME}"
    LANGUAGES C
)

add_definitions(-DSDT)
add_definitions(-U__linux__)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")


# Add subdirectories
add_subdirectory(sc_libs)
add_subdirectory(xlnx_libs)

# Add main executable
add_executable(${PROJECT_NAME}
    main.c
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    xlnx_libs
    sc_libs
    m
)

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/xlnx_libs
    ${CMAKE_CURRENT_SOURCE_DIR}/sc_libs
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Enable testing
enable_testing() 