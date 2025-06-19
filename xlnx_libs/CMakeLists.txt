cmake_minimum_required(VERSION 3.10)

# Define base paths
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SC_LIBS_DIR ${SRC_DIR}/../sc_libs)
set(PROJECT_ROOT ${SRC_DIR}/..)

# Gather source files recursively
file(GLOB_RECURSE XLNX_LIB_SRC
    ${SRC_DIR}/standalone/*.c
    ${SRC_DIR}/gpio/*.c
    ${SRC_DIR}/sysmon/*.c
    ${SRC_DIR}/tmrctr/*.c
)

# Create the static library
add_library(xlnx_libs STATIC ${XLNX_LIB_SRC})

# Link math library (standard C math lib) and sc_libs
target_link_libraries(xlnx_libs PUBLIC m sc_libs)

# Set include directories
target_include_directories(xlnx_libs PUBLIC
    ${SRC_DIR}/standalone
    ${SRC_DIR}/hw_config
    ${SRC_DIR}/gpio
    ${SRC_DIR}/sysmon
    ${SRC_DIR}/tmrctr
    ${SC_LIBS_DIR}
    ${PROJECT_ROOT}
)

# Debug: Print all gathered source files
message(STATUS "xlnx_libs sources:")
foreach(src_file ${XLNX_LIB_SRC})
    message(STATUS "  ${src_file}")
endforeach()
