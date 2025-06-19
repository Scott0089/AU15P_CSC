/**
 * @file sc_io.h
 * @brief I/O operations for FPGA device interface
 * @version 1.0.0
 * @date 2024
 *
 * This header provides functions for reading and writing to FPGA device registers
 * through a memory-mapped interface. It includes error handling and address validation.
 */

#ifndef SC_IO_H
#define SC_IO_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Status codes for I/O operations
 */
typedef enum {
    SC_IO_SUCCESS = 0,           /**< Operation completed successfully */
    SC_IO_ERROR_READ = -1,       /**< Error during read operation */
    SC_IO_ERROR_WRITE = -2,      /**< Error during write operation */
    SC_IO_ERROR_INVALID_ADDR = -3 /**< Invalid address specified */
} sc_io_status_t;

/**
 * @brief Read a 32-bit value from a device register
 *
 * @param addr The address to read from
 * @param data Pointer to store the read value
 * @return sc_io_status_t Status of the operation
 */
sc_io_status_t SC_In32(uintptr_t addr, uint32_t *data);

/**
 * @brief Write a 32-bit value to a device register
 *
 * @param addr The address to write to
 * @param data The value to write
 * @return sc_io_status_t Status of the operation
 */
sc_io_status_t SC_Out32(uintptr_t addr, uint32_t data);

/**
 * @brief Check if an address is valid for I/O operations
 *
 * @param addr The address to validate
 * @return true if the address is valid, false otherwise
 */
bool SC_IsValidAddr(uintptr_t addr);

#endif /* SC_IO_H */
