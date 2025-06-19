# FPGA Device Interface

A template project for FPGA-based applications using XDMA for host-FPGA communication.

## Prerequisites

- CMake 3.10 or higher
- C compiler with C11 support
- Xilinx XDMA driver installed
- Required libraries in `sc_libs` and `xlnx_libs` directories

## Building the Project

1. Clone the repository:
```bash
git clone [repository-url]
cd [repository-name]
```

2. Create and enter build directory:
```bash
mkdir build
cd build
```

3. Configure and build:
```bash
cmake ..
make
```

The executable will be created in `build/bin/AU15P_CSC2`

## Running the Application

The application requires root privileges to access the XDMA device:

```bash
sudo ./build/bin/AU15P_CSC2
```

## Project Structure

```
.
├── main.c              # Main application code
├── main.h              # Header file with common definitions
├── sc_libs/           # Custom library implementations
│   ├── sc_io.c        # I/O operations implementation
│   └── sc_io.h        # I/O operations interface
├── xlnx_libs/         # Xilinx library dependencies
└── CMakeLists.txt     # Build configuration
```

