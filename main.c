#include "main.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define RX_ENABLE

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define PIXELS_PER_WORD 2
#define WORD_SIZE 8
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
XGpio GTCtrl;
XGpio ClkGenRst;
XGpio SfpTxDis;
XGpio SdiTxRst;

XIic  iicInst;
XIic_Config *XIic_ConfigPtr;

XV_tpg tpgInst;
XV_tpg_Config *tpg_ConfigPtr;

XVprocSs vprocInst;
XVprocSs_Config *vproc_ConfigPtr;

XVidC_VideoMode resId;
XVidC_VideoTiming const *timingPtr;
XVidC_VideoStream *streamOut;

XV_SdiTxSs  sdiTxSsInst;
XV_SdiTx_Config *sdiTxSs_ConfigPtr;

XVidC_VideoStream *sdiTxSsVidStreamPtr;
XSdiVid_Transport *sdiTxSsTransportPtr;

XV_SdiRxSs  sdiRxSsInst;
XV_SdiRx_Config *sdiRxSs_ConfigPtr;

uint32_t Iic_Scan(XIic *InstancePtr)
{
    uint8_t buffer[2];
    uint8_t bytesRecvd;
    uint8_t found = 0;

    printf("Scanning I2C Bus! \r\n");

    for (size_t i = 0; i < 128; i++)
    {
        bytesRecvd = XIic_Recv(InstancePtr->BaseAddress, i, (uint8_t*) buffer, 1, XIIC_STOP);
        if(bytesRecvd == 0)
        {
            continue;
        }
        printf("Found Device at Address: 0x%02x\r\n", i);
        found++;
    }
    
    if(found == 0)
    {
        printf("No I2C Devices Found! \r\n");
    }
    else
    {
        printf("Number of I2C Devices Found: %d\r\n", found);
    }

    return XST_SUCCESS;  
}

uint32_t GpioInit()
{
    uint32_t status;

    status = XGpio_Initialize(&TpgReset, XPAR_TPG_TPG_RESET_BASEADDR);
    if(status != XST_SUCCESS)
    {
        printf("Failed to Init TPG RST GPIO! \r\n");
        return XST_FAILURE;
    }
    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x01); //Out of RST

    status = XGpio_Initialize(&GTCtrl, XPAR_SDI_GT_CTRL_GT_CTRL_GPIO_BASEADDR);
    if(status != XST_SUCCESS)
    {
        printf("Failed to Init GT CTRL GPIO! \r\n");
        return XST_FAILURE;
    }
    XGpio_DiscreteWrite(&GTCtrl, 0x01, 0x00);

    status = XGpio_Initialize(&ClkGenRst, XPAR_SDI_IO_CLKGEN_RST_N_BASEADDR);
    if(status != XST_SUCCESS)
    {
        printf("Failed to Init Clk Gen RST GPIO! \r\n");
        return XST_FAILURE;
    }
    XGpio_DiscreteWrite(&ClkGenRst, 0x01, 0x01);

    status = XGpio_Initialize(&SfpTxDis, XPAR_SDI_IO_SFP1_TDIS_BASEADDR);
    if(status != XST_SUCCESS)
    {
        printf("Failed to Init SFP TX Dis GPIO! \r\n");
        return XST_FAILURE;
    }
    XGpio_DiscreteWrite(&SfpTxDis, 0x01, 0x00);

    status = XGpio_Initialize(&SdiTxRst, XPAR_SDI_SDI_TX_SDI_TX_RST_BASEADDR);
    if(status != XST_SUCCESS)
    {
        printf("Failed to Init SDI TX RST GPIO! \r\n");
        return XST_FAILURE;
    }
    XGpio_DiscreteWrite(&SdiTxRst, 0x01, 0x01);
    
    return XST_SUCCESS;
}

uint32_t IicInit()
{
    uint32_t status;

    XIic_ConfigPtr = XIic_LookupConfig(XPAR_SDI_IO_CLKGEN_IIC_BASEADDR);
    if(XIic_ConfigPtr == NULL)
    {
        printf("Failed to get CLK GEN IIC Config! \r\n");
        return XST_FAILURE;
    }
    status = XIic_CfgInitialize(&iicInst, XIic_ConfigPtr, XIic_ConfigPtr->BaseAddress);
    if(status != XST_SUCCESS)
    {
        printf("Failed to Init CLK GEN IIC! \r\n");
        return XST_FAILURE;
    }

    return status;
}

uint32_t TpgInit()
{
    uint32_t status;

    tpg_ConfigPtr = XV_tpg_LookupConfig(XPAR_TPG_V_TPG_0_BASEADDR);
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
    //XV_tpg_Start(&tpgInst);

    printf("TPG Started! \r\n");

    return status;
}

uint32_t HW_Init()
{
    uint32_t status;

    GpioInit();
    IicInit();
    TpgInit();

    return XST_SUCCESS;
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
        } else {
            printf("Failed to read a full frame from XDMA (read %zd bytes)\n", total_read);
        }
    }

    munmap(shm_ptr, TOTAL_SIZE);
    close(in_fd);
    close(shm_fd);
    return 0;
}

void poll_tx_sdi_irq(uint32_t irq)
{
    uint32_t status = 1;
    uint32_t isr_reg;
    
    while(status)
    {
        isr_reg = XV_SdiTx_GetIntrStatus(sdiTxSsInst.SdiTxPtr); //might be right
        if((isr_reg & irq) > 0)
        {
            status = 0;
            printf("SDI TX Interrupt: %x detected! \r\n", isr_reg);
        }
        
        if(isr_reg > 0)
        {
            XV_SdiTx_InterruptClear(sdiTxSsInst.SdiTxPtr, isr_reg);
            printf("SDI TX Interrupt Cleared: %x! \r\n", isr_reg);
        }
    }
}

void poll_rx_sdi_irq(uint32_t irq)
{
    uint32_t status = 1;
    uint32_t isr_reg;
    
    while(status)
    {
        isr_reg = XV_SdiRx_GetIntrStatus(sdiRxSsInst.SdiRxPtr); //might be right
        if((isr_reg & irq) > 0)
        {
            status = 0;
            printf("SDI RX Interrupt: %x detected! \r\n", isr_reg);
        }
        
        if(isr_reg > 0)
        {
            XV_SdiRx_InterruptClear(sdiRxSsInst.SdiRxPtr, isr_reg);
            printf("SDI RX Interrupt Cleared: %x! \r\n", isr_reg);
        }
    } 
}

void config_sdi_tx()
{
    sdiTxSsTransportPtr = XV_SdiTxSs_GetTransport(&sdiTxSsInst);

    sdiTxSsVidStreamPtr = XV_SdiTxSs_GetVideoStream(&sdiTxSsInst, 0);
    XVidC_SetVideoStream(sdiTxSsVidStreamPtr, XVIDC_VM_1920x1080_60_P, XVIDC_CSF_YCBCR_422, XVIDC_BPC_10, XVIDC_PPC_2);
    XV_SdiTx_GetPayload(sdiTxSsInst.SdiTxPtr, XVIDC_VM_1920x1080_60_P, XSDIVID_MODE_3GA, 0);

    sdiTxSsTransportPtr->TMode = XSDIVID_MODE_3GA;
    sdiTxSsTransportPtr->IsFractional = 0x00;
    sdiTxSsTransportPtr->IsLevelB3G = 0x00;
    sdiTxSsTransportPtr->TScan  = 0x01;

    const XVidC_VideoTiming *timing = XVidC_GetTimingInfo(XVIDC_VM_1920x1080_60_P);

    XV_tpg_Set_height(&tpgInst, timing->VActive);
    XV_tpg_Set_width(&tpgInst, timing->HActive);
}

uint32_t config_sdi_rx()
{
    XSdiVid_TransMode RxTransMode;
    uint8_t isFractional;
    XVidC_VideoStream *sdiRxSsVidStreamPtr = NULL;
    const XVidC_VideoTiming *timingPtr = NULL;
    XVidC_FrameRate Frame_Rate;
    uint32_t status;

    vproc_ConfigPtr = XVprocSs_LookupConfig(XPAR_XVPROCSS_0_BASEADDR);
    if(vproc_ConfigPtr == NULL)
    {
        perror("Failed to get Video Processing Config! \r\n");
        return XST_FAILURE;
    }

    status = XVprocSs_CfgInitialize(&vprocInst, vproc_ConfigPtr, vproc_ConfigPtr->BaseAddress);
    printf("vproc_ConfigPtr: %p\n", vproc_ConfigPtr);
    if (vproc_ConfigPtr) {
        printf("vproc_ConfigPtr->BaseAddress: 0x%08X\n", vproc_ConfigPtr->BaseAddress);
        printf("vproc_ConfigPtr->ColorDepth: %d\n", vproc_ConfigPtr->ColorDepth);
        printf("vproc_ConfigPtr->PixPerClock: %d\n", vproc_ConfigPtr->PixPerClock);
    }
    printf("vprocInst: %p\n", &vprocInst);
    printf("XVprocSs_CfgInitialize returned: 0x%08X\n", status);
    if(status != XST_SUCCESS)
    {
        perror("Failed to Init Video Processing Subsystem! \r\n");
        return XST_FAILURE;
    }

    resId = XVidC_GetVideoModeId(FRAME_WIDTH, FRAME_HEIGHT, XVIDC_FR_60HZ, 0);
    timingPtr = XVidC_GetTimingInfo(resId);
    
    sdiRxSsVidStreamPtr = XV_SdiRxSs_GetVideoStream(&sdiRxSsInst, 0);
    timingPtr = &sdiRxSsVidStreamPtr->Timing;

    XVidC_ReportStreamInfo(sdiRxSsVidStreamPtr);
    XVprocSs_SetVidStreamIn(&vprocInst, sdiRxSsVidStreamPtr);

    streamOut = XV_SdiRxSs_GetVideoStream(&sdiRxSsInst, 0);
    streamOut->ColorFormatId = XVIDC_CSF_YCRCB_422;

    XVprocSs_SetVidStreamOut(&vprocInst, streamOut);

    printf("Input Stream: %dx%d, ColorFormat: %d, Depth: %d, PPC: %d, FR: %d, Interlaced: %d\n",
        sdiRxSsVidStreamPtr->Timing.HActive,
        sdiRxSsVidStreamPtr->Timing.VActive,
        sdiRxSsVidStreamPtr->ColorFormatId,
        sdiRxSsVidStreamPtr->ColorDepth,
        sdiRxSsVidStreamPtr->PixPerClk,
        sdiRxSsVidStreamPtr->FrameRate,
        sdiRxSsVidStreamPtr->IsInterlaced);

    status = XVprocSs_SetSubsystemConfig(&vprocInst);
    if(status != XST_SUCCESS)
    {
        printf("XVprocSs_SetSubsystemConfig returned: 0x%08X\n", status);
        perror("Failed to Start Video Processing Subsystem! \r\n");
        return XST_FAILURE;
    }

    printf("Video Processing Subsystem Started! \r\n");

    XV_SdiRxSs_StreamFlowEnable(&sdiRxSsInst);
}

void start_sdi_tx()
{
    XV_SdiTxSs_StreamStart(&sdiTxSsInst);
    XV_SdiTxSs_StreamConfig(&sdiTxSsInst);

    XV_tpg_Set_colorFormat(&tpgInst, XVIDC_CSF_YCRCB_422);
    XV_tpg_Set_maskId(&tpgInst, 0x00);
    XV_tpg_Set_motionSpeed(&tpgInst, 5);
    XV_tpg_Set_motionEn(&tpgInst, 1);
    XV_tpg_Set_bckgndId(&tpgInst, XTPG_BKGND_COLOR_BARS);
    
    XV_tpg_Set_boxColorB(&tpgInst, 0x00);
    XV_tpg_Set_boxColorR(&tpgInst, 0x00);
    XV_tpg_Set_boxColorG(&tpgInst, 0x00);
    XV_tpg_Set_boxSize(&tpgInst, 50);
    
    XV_tpg_Set_ovrlayId(&tpgInst, 0x01);

    XV_tpg_EnableAutoRestart(&tpgInst);
    XV_tpg_Start(&tpgInst);

}

uint32_t SdiTxInit()
{
    uint32_t status;

    XGpio_DiscreteWrite(&SdiTxRst, 0x01, 0x00);

    printf("Setting Up SDI TX! \r\n");

    //SDI TX
    sdiTxSs_ConfigPtr = XV_SdiTxSs_LookupConfig(XPAR_SDI_SDI_TX_V_SMPTE_UHDSDI_TX_SS_0_BASEADDR);
    if(sdiTxSs_ConfigPtr == NULL)
    {
        printf("Failed to get SDI TX Config! \r\n");
        return XST_FAILURE;
    }

    status = XV_SdiTxSs_CfgInitialize(&sdiTxSsInst, sdiTxSs_ConfigPtr, sdiTxSs_ConfigPtr->BaseAddress);
    if (status != XST_SUCCESS)
    {
        printf("Failed to Init SDI TX! \r\n");
        return XST_FAILURE;
    }

    XV_SdiTxSs_IntrDisable(&sdiTxSsInst, XV_SDITXSS_IER_ALLINTR_MASK);
    printf("Disabled SDI TX Interrupts! \r\n");

    printf("Finished Setting Up SDI TX! \r\n");

    TpgInit();

    return status;
}

uint32_t SdiRxInit()
{
    uint32_t status;

    printf("Setting Up SDI RX! \r\n");

    //SDI RX
    sdiRxSs_ConfigPtr = XV_SdiRxSs_LookupConfig(XPAR_SDI_SDI_RX_V_SMPTE_UHDSDI_RX_SS_0_BASEADDR);
    if(sdiRxSs_ConfigPtr == NULL)
    {
        printf("Failed to get SDI RX Config! \r\n");
        return XST_FAILURE;
    }

    status = XV_SdiRxSs_CfgInitialize(&sdiRxSsInst, sdiRxSs_ConfigPtr, sdiRxSs_ConfigPtr->BaseAddress);
    if (status != XST_SUCCESS)
    {
        printf("Failed to Init SDI RX! \r\n");
        return XST_FAILURE;
    }

    XV_SdiRxSs_IntrDisable(&sdiRxSsInst, XV_SDIRXSS_IER_ALLINTR_MASK);
    printf("Disabled SDI RX Interrupts! \r\n");

    printf("Finished Setting Up SDI RX! \r\n");
    return XST_SUCCESS;
}

uint32_t SdiInit()
{
    uint32_t isr_reg;
    uint32_t status;

    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x00);
    usleep(300000);
    XGpio_DiscreteWrite(&TpgReset, 0x01, 0x01);

    XGpio_DiscreteWrite(&GTCtrl, 0x01, 0x00);
    usleep(300000);
    XGpio_DiscreteWrite(&GTCtrl, 0x01, 0x02);

    printf("Resetting Video! \r\n");

    poll_tx_sdi_irq(XV_SDITX_IER_GTTX_RSTDONE_MASK);

    XV_SdiTxSs_StreamStart(&sdiTxSsInst);
    printf("Started SDI Stream! \r\n");

    config_sdi_tx();
    start_sdi_tx();

    poll_rx_sdi_irq(XV_SDIRXSS_IER_VIDEO_LOCK_MASK);
    SH_SdiRx_VidLckIntrHandler_poll(sdiRxSsInst.SdiRxPtr);

    status = config_sdi_rx();
    if(status != XST_SUCCESS)
    {
        perror("Major Error Boss! \r\n");
        return XST_FAILURE;
    }

    XV_SdiTxSs_ReportInfo(&sdiTxSsInst);
    XV_SdiTxSs_ReportDebugInfo(&sdiTxSsInst, 1);

    return status;
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

    status = HW_Init();
    if(status != XST_SUCCESS)
    {
        printf("Failed to Init HW! \r\n");
    }

    Iic_Scan(&iicInst);
    //usleep(300000);
    IDT_8T49N24x_Init(iicInst.BaseAddress, 0x7C);
    printf("Clk Gen Configured! \r\n");

    SdiTxInit();
    SdiRxInit();
    status = SdiInit();
    if(status != XST_SUCCESS)
    {
        perror("Super big error Boss! \r\n");
        return XST_FAILURE;
    }

    XV_SdiRxSs_ReportInfo(&sdiRxSsInst);
    XV_SdiRxSs_ReportDebugInfo(&sdiRxSsInst);

    printf("SDI Started! \r\n");

    Producer();

    sleep(1);

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

