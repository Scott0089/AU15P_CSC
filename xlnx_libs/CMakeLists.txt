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
    ${SRC_DIR}/v_tpg/*.c
    ${SRC_DIR}/video_common/*.c
    
    ${SRC_DIR}/vidproc/axis_switch/*.c
    ${SRC_DIR}/vidproc/axivdma/*.c
    ${SRC_DIR}/vidproc/v_csc/*.c
    ${SRC_DIR}/vidproc/v_deinterlacer/*.c
    ${SRC_DIR}/vidproc/v_hcresampler/*.c
    ${SRC_DIR}/vidproc/v_hscaler/*.c
    ${SRC_DIR}/vidproc/v_letterbox/*.c
    ${SRC_DIR}/vidproc/v_vcresampler/*.c
    ${SRC_DIR}/vidproc/v_vscaler/*.c
    ${SRC_DIR}/vidproc/vprocss/*.c
    ${SRC_DIR}/vidproc/vprocss/vdma/*.c
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
    ${SRC_DIR}/v_tpg
    ${SRC_DIR}/video_common
    
    ${SRC_DIR}/vidproc
    ${SRC_DIR}/vidproc/axis_switch
    ${SRC_DIR}/vidproc/axivdma
    ${SRC_DIR}/vidproc/v_csc
    ${SRC_DIR}/vidproc/v_deinterlacer
    ${SRC_DIR}/vidproc/v_hcresampler
    ${SRC_DIR}/vidproc/v_hscaler
    ${SRC_DIR}/vidproc/v_letterbox
    ${SRC_DIR}/vidproc/v_vcresampler
    ${SRC_DIR}/vidproc/v_vscaler
    ${SRC_DIR}/vidproc/vprocss
    ${SRC_DIR}/vidproc/vprocss/vdma
   
    
    ${SC_LIBS_DIR}
    ${PROJECT_ROOT}
)

# Debug: Print all gathered source files
message(STATUS "xlnx_libs sources:")
foreach(src_file ${XLNX_LIB_SRC})
    message(STATUS "  ${src_file}")
endforeach()
