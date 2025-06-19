# Minimum required version
cmake_minimum_required(VERSION 3.10)

# Create the static library from all C source files in this directory
file(GLOB SC_LIBS_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

add_library(sc_libs STATIC ${SC_LIBS_SRC})

# Set library properties
set_target_properties(sc_libs PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "sc_io.h"
)

# Include directories needed by sc_libs
target_include_directories(sc_libs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../xlnx_libs/standalone
        ${CMAKE_CURRENT_SOURCE_DIR}/../xlnx_libs/hw_config
        ${CMAKE_CURRENT_SOURCE_DIR}/../xlnx_libs/gpio
        ${CMAKE_CURRENT_SOURCE_DIR}/../xlnx_libs/sysmon
        ${CMAKE_CURRENT_SOURCE_DIR}/../xlnx_libs/tmrctr
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link with math library
target_link_libraries(sc_libs PUBLIC m)

# Install rules
install(TARGETS sc_libs
    EXPORT sc_libs-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT sc_libs-targets
    FILE sc_libs-targets.cmake
    NAMESPACE sc_libs::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sc_libs
)
