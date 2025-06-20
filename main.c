#include "main.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define PIXELS_PER_WORD 2
#define WORD_SIZE 5 // 40 bits = 5 bytes
#define TOTAL_PIXELS (FRAME_WIDTH * FRAME_HEIGHT)
#define TOTAL_WORDS (TOTAL_PIXELS / PIXELS_PER_WORD)
#define HEADER_SIZE 4
#define FRAME_SIZE_BYTES (TOTAL_WORDS * WORD_SIZE)
#define TOTAL_SIZE (HEADER_SIZE + FRAME_SIZE_BYTES)

#define SHM_PATH "/xdma_buffer"
#define XDMA_PATH "/dev/xdma0_c2h_0"

/* Global variables */
int fd = -1;

XGpio VersionCheck;
XGpio TpgReset;

XV_tpg tpgInst;
XV_tpg_Config *tpg_ConfigPtr;

XVprocSs vprocInst;
XVprocSs_Config *vproc_ConfigPtr;

XVidC_VideoMode resId;
XVidC_VideoTiming const *timingPtr;
XVidC_VideoStream streamIn, streamOut;

uint32_t TpgInit()
{
    uint32_t status;

    tpg_ConfigPtr = XV_tpg_LookupConfig(XPAR_V_TPG_0_BASEADDR);
    if(tpg_ConfigPtr == NULL)
    {
        perror("Failed to get TPG Config! \r\n");
        return XST_FAILURE;
    }

    status = XV_tpg_CfgInitialize(&tpgInst, tpg_ConfigPtr, tpg_ConfigPtr->BaseAddress);
    if (status != XST_SUCCESS)
    {
        perror("Failed to Init TPG! \r\n");
        return XST_FAILURE;
    }

    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x01);
    usleep(300000);
    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x00);
    usleep(300000);
    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x01);
    
    printf("TPG Init Success! \r\n");

    XV_tpg_Set_height(&tpgInst, FRAME_HEIGHT);
    XV_tpg_Set_width(&tpgInst, FRAME_WIDTH);
    XV_tpg_Set_colorFormat(&tpgInst, XVIDC_CSF_YCRCB_422);
    XV_tpg_Set_maskId(&tpgInst, 0x00);
    XV_tpg_Set_motionSpeed(&tpgInst, 1);
    XV_tpg_Set_motionEn(&tpgInst, 1);
    XV_tpg_Set_bckgndId(&tpgInst, XTPG_BKGND_COLOR_BARS);
    
    XV_tpg_Set_boxColorB(&tpgInst, 0xFF);
    XV_tpg_Set_boxColorR(&tpgInst, 0xFF);
    XV_tpg_Set_boxColorG(&tpgInst, 0xFF);
    XV_tpg_Set_boxSize(&tpgInst, 50);
    
    XV_tpg_Set_ovrlayId(&tpgInst, 0x01);

    XV_tpg_EnableAutoRestart(&tpgInst);
    XV_tpg_Start(&tpgInst);

    printf("TPG Started! \r\n");

    return status;
}

uint32_t VProcInit()
{

    uint32_t status;

    vproc_ConfigPtr = XVprocSs_LookupConfig(XPAR_XVPROCSS_0_BASEADDR);
    if(vproc_ConfigPtr == NULL)
    {
        perror("Failed to get Video Processing Config! \r\n");
        return XST_FAILURE;
    }

    status = XVprocSs_CfgInitialize(&vprocInst, vproc_ConfigPtr, vproc_ConfigPtr->BaseAddress);
    if(status != XST_SUCCESS)
    {
        perror("Failed to Init Video Processing Subsystem! \r\n");
        return XST_FAILURE;
    }

    resId = XVidC_GetVideoModeId(FRAME_WIDTH, FRAME_HEIGHT, XVIDC_FR_60HZ, 0);
    timingPtr = XVidC_GetTimingInfo(resId);

    streamIn.VmId = resId;
    streamIn.Timing = *timingPtr;
    streamIn.ColorFormatId = XVIDC_CSF_YCRCB_422;
    streamIn.ColorDepth = vproc_ConfigPtr->ColorDepth;
    streamIn.PixPerClk = vproc_ConfigPtr->PixPerClock;
    streamIn.FrameRate = XVIDC_FR_60HZ;
    streamIn.IsInterlaced = 0x00;
    XVprocSs_SetVidStreamIn(&vprocInst, &streamIn);

    streamOut.VmId = resId;
    streamOut.Timing = *timingPtr;
    streamOut.ColorFormatId = XVIDC_CSF_YCRCB_422;
    streamOut.ColorDepth = vproc_ConfigPtr->ColorDepth;
    streamOut.PixPerClk = vproc_ConfigPtr->PixPerClock;
    streamOut.FrameRate = XVIDC_FR_60HZ;
    streamOut.IsInterlaced = 0x00;
    XVprocSs_SetVidStreamOut(&vprocInst, &streamOut);

    status = XVprocSs_SetSubsystemConfig(&vprocInst);
    if(status != XST_SUCCESS)
    {
        perror("Failed to Start Video Processing Subsystem! \r\n");
        return XST_FAILURE;
    }

    printf("Video Processing Subsystem Started! \r\n");
    
    return status;
}

uint32_t Producer() {
    int in_fd, shm_fd;
    ssize_t bytes_read, total_read;
    uint8_t *shm_ptr;

    // Open XDMA input device
    in_fd = open(XDMA_PATH, O_RDONLY);
    if (in_fd < 0) {
        perror("Failed to open XDMA input");
        return 1;
    }

    // Create and open shared memory object
    shm_fd = shm_open(SHM_PATH, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("Failed to open shared memory object");
        close(in_fd);
        return 1;
    }

    // Set size of shared memory
    if (ftruncate(shm_fd, TOTAL_SIZE) == -1) {
        perror("Failed to set size of shared memory");
        close(in_fd);
        close(shm_fd);
        return 1;
    }

    // Map shared memory
    shm_ptr = mmap(NULL, TOTAL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Failed to mmap shared memory");
        close(in_fd);
        close(shm_fd);
        return 1;
    }

    uint32_t *frame_counter = (uint32_t *)shm_ptr;
    uint8_t *frame_data = shm_ptr + HEADER_SIZE;
    *frame_counter = 0; // Initialize frame counter

    while (1) {
        total_read = 0;

        // 1. Set counter to odd (writing)
        // Only increment counter if we are about to write a new frame
        // (move this after successful read)

        // 2. Read one full frame into frame_data
        while (total_read < FRAME_SIZE_BYTES) {
            bytes_read = read(in_fd, frame_data + total_read, FRAME_SIZE_BYTES - total_read);
            if (bytes_read < 0) {
                perror("Error reading from XDMA");
                break;
            } else if (bytes_read == 0) {
                fprintf(stderr, "Unexpected EOF from XDMA\n");
                break;
            }
            total_read += bytes_read;
        }

        if (total_read == FRAME_SIZE_BYTES) {
            // 1. Set counter to odd (writing)
            (*frame_counter)++;
            __sync_synchronize(); // Memory barrier

            // 3. Set counter to even (ready)
            (*frame_counter)++;
            __sync_synchronize(); // Memory barrier
            printf("Frame written, counter: %u\n", *frame_counter);
        } else {
            // If we didn't get a full frame, don't update the counter
            printf("Failed to read a full frame from XDMA (read %zd bytes)\n", total_read);
        }
    }

    munmap(shm_ptr, TOTAL_SIZE);
    close(in_fd);
    close(shm_fd);
    return 0;
}

int main()
{
    uint32_t data;
    uint32_t status;

    // Open the XDMA device
    fd = open("/dev/xdma0_user", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/xdma0_user");
        printf("Check if driver is installed.\n");
        return XST_FAILURE;
    }

    status = XGpio_Initialize(&VersionCheck, XPAR_SYS_CONFIG_VERSION_CHECK_BASEADDR);
    if(status != XST_SUCCESS)
    {
        perror("Version Check GPIO Failed to Init! \r\n");
        return XST_FAILURE;
    }

    printf("\n");
    printf("                            AU15P CSC2\n");
    printf("                 -------------------------------\n");
    printf("                   Color Space Conversion v2.0\n");
    printf("                 -------------------------------\n\n");

    printf("Version ID: 0x%08X\r\n", XGpio_DiscreteRead(&VersionCheck, 0x01));

    status = XGpio_Initialize(&TpgReset, XPAR_AXI_GPIO_0_BASEADDR);
    if(status != XST_SUCCESS)
    {
        perror("TPG RESET GPIO Failed to Init! \r\n");
        return XST_FAILURE;
    }

    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x01);


    TpgInit();
    VProcInit();

    Producer();

    printf("\nTest completed successfully.\n");

    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x01);
    usleep(300000);
    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x00);
    usleep(300000);
    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x01);

    // Clean up
    if (close(fd) < 0) {
        perror("Failed to close device");
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

