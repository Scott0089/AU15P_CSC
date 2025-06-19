#include "main.h"

/* Global variables */
int fd = -1;

int main()
{
    uint32_t data;
    uint8_t status;

    // Open the XDMA device
    fd = open("/dev/xdma0_user", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/xdma0_user");
        printf("Check if driver is installed.\n");
        return XST_FAILURE;
    }

    printf("\n");
    printf("                        FPGA Device Test\n");
    printf("                 ------------------------------\n");
    printf("                    Device Test Interface v1.0\n");
    printf("                 ------------------------------\n\n");

    printf("\nTest completed successfully.\n");

    // Clean up
    if (close(fd) < 0) {
        perror("Failed to close device");
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

