// ==============================================================
// Copyright (c) 2015 - 2020 Xilinx Inc. All rights reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// SPDX-License-Identifier: MIT
// ==============================================================

#ifndef XV_DEINTERLACER_H
#define XV_DEINTERLACER_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__dep
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xv_deinterlacer_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__dep
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
/**
* This typedef contains configuration information for the DEINT core.
* Each core instance should have a configuration structure
* associated.
*/
typedef struct {
#ifndef SDT
    u16 DeviceId;              /**< Unique ID  of device */
#else
    char *Name;
#endif
    UINTPTR BaseAddress;       /**< The base address of the core instance. */
    u16 NumVidComponents;      /**< Number of Video Components */
    u16 MaxDataWidth;          /**< Maximum Data width of each channel */
} XV_deinterlacer_Config;
#endif

/**
* Driver instance data. An instance must be allocated for each core in use.
*/
typedef struct {
    XV_deinterlacer_Config Config; /**< Hardware Configuration */
    u32 IsReady;                   /**< Device is initialized and ready */
} XV_deinterlacer;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__dep
#define XV_deinterlacer_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XV_deinterlacer_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XV_deinterlacer_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XV_deinterlacer_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__dep
#ifndef SDT
int XV_deinterlacer_Initialize(XV_deinterlacer *InstancePtr, u16 DeviceId);
XV_deinterlacer_Config* XV_deinterlacer_LookupConfig(u16 DeviceId);
#else
int XV_deinterlacer_Initialize(XV_deinterlacer *InstancePtr, UINTPTR BaseAddress);
XV_deinterlacer_Config* XV_deinterlacer_LookupConfig(UINTPTR BaseAddress);
#endif
int XV_deinterlacer_CfgInitialize(XV_deinterlacer *InstancePtr,
                                  XV_deinterlacer_Config *ConfigPtr,
								  UINTPTR EffectiveAddr);
#else
int XV_deinterlacer_Initialize(XV_deinterlacer *InstancePtr, const char* InstanceName);
int XV_deinterlacer_Release(XV_deinterlacer *InstancePtr);
#endif

void XV_deinterlacer_Start(XV_deinterlacer *InstancePtr);
u32 XV_deinterlacer_IsDone(XV_deinterlacer *InstancePtr);
u32 XV_deinterlacer_IsIdle(XV_deinterlacer *InstancePtr);
u32 XV_deinterlacer_IsReady(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_EnableAutoRestart(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_DisableAutoRestart(XV_deinterlacer *InstancePtr);

void XV_deinterlacer_Set_width(XV_deinterlacer *InstancePtr, u32 Data);
u32 XV_deinterlacer_Get_width(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_Set_height(XV_deinterlacer *InstancePtr, u32 Data);
u32 XV_deinterlacer_Get_height(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_Set_read_fb(XV_deinterlacer *InstancePtr, u64 Data);
u64 XV_deinterlacer_Get_read_fb(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_Set_write_fb(XV_deinterlacer *InstancePtr, u64 Data);
u64 XV_deinterlacer_Get_write_fb(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_Set_colorFormat(XV_deinterlacer *InstancePtr, u32 Data);
u32 XV_deinterlacer_Get_colorFormat(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_Set_algo(XV_deinterlacer *InstancePtr, u32 Data);
u32 XV_deinterlacer_Get_algo(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_Set_invert_field_id(XV_deinterlacer *InstancePtr, u32 Data);
u32 XV_deinterlacer_Get_invert_field_id(XV_deinterlacer *InstancePtr);

void XV_deinterlacer_InterruptGlobalEnable(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_InterruptGlobalDisable(XV_deinterlacer *InstancePtr);
void XV_deinterlacer_InterruptEnable(XV_deinterlacer *InstancePtr, u32 Mask);
void XV_deinterlacer_InterruptDisable(XV_deinterlacer *InstancePtr, u32 Mask);
void XV_deinterlacer_InterruptClear(XV_deinterlacer *InstancePtr, u32 Mask);
u32 XV_deinterlacer_InterruptGetEnabled(XV_deinterlacer *InstancePtr);
u32 XV_deinterlacer_InterruptGetStatus(XV_deinterlacer *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
