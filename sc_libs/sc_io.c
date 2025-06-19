#include "sc_io.h"
#include "main.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


/* Address validation */
bool SC_IsValidAddr(uintptr_t addr) {
    // TODO: Add proper address range checking
    return (addr != 0);
}

/* Read 32-bit value from address */
sc_io_status_t SC_In32(uintptr_t addr, uint32_t *data) {
    if (!data || !SC_IsValidAddr(addr)) {
        return SC_IO_ERROR_INVALID_ADDR;
    }

    // Subtract base address to get offset
    uintptr_t offset = addr - TARGET_PHYS_ADDR;
    ssize_t bytes_read = pread(fd, data, sizeof(uint32_t), offset);
    if (bytes_read != sizeof(uint32_t)) {
        return SC_IO_ERROR_READ;
    }

    return SC_IO_SUCCESS;
}

/* Write 32-bit value to address */
sc_io_status_t SC_Out32(uintptr_t addr, uint32_t data) {
    if (!SC_IsValidAddr(addr)) {
        return SC_IO_ERROR_INVALID_ADDR;
    }

    // Subtract base address to get offset
    uintptr_t offset = addr - TARGET_PHYS_ADDR;
    ssize_t bytes_written = pwrite(fd, &data, sizeof(uint32_t), offset);
    if (bytes_written != sizeof(uint32_t)) {
        return SC_IO_ERROR_WRITE;
    }

    return SC_IO_SUCCESS;
}
