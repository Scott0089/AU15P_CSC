// ==============================================================
// Copyright (c) 2015 - 2020 Xilinx Inc. All rights reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// SPDX-License-Identifier: MIT
// ==============================================================

#ifndef __linux__dep

#include "xstatus.h"
#ifndef SDT
#include "xparameters.h"
#endif
#include "xv_vscaler.h"

#ifndef XPAR_XV_VSCALER_NUM_INSTANCES
#define XPAR_XV_VSCALER_NUM_INSTANCES   0
#endif

extern XV_vscaler_Config XV_vscaler_ConfigTable[];

#ifndef SDT
XV_vscaler_Config *XV_vscaler_LookupConfig(u16 DeviceId) {
    XV_vscaler_Config *ConfigPtr = NULL;

    int Index;

    for (Index = 0; Index < XPAR_XV_VSCALER_NUM_INSTANCES; Index++) {
        if (XV_vscaler_ConfigTable[Index].DeviceId == DeviceId) {
            ConfigPtr = &XV_vscaler_ConfigTable[Index];
            break;
        }
    }

    return ConfigPtr;
}

int XV_vscaler_Initialize(XV_vscaler *InstancePtr, u16 DeviceId) {
    XV_vscaler_Config *ConfigPtr;

    Xil_AssertNonvoid(InstancePtr != NULL);

    ConfigPtr = XV_vscaler_LookupConfig(DeviceId);
    if (ConfigPtr == NULL) {
        InstancePtr->IsReady = 0;
        return (XST_DEVICE_NOT_FOUND);
    }

    return XV_vscaler_CfgInitialize(InstancePtr,
                                    ConfigPtr,
                                    ConfigPtr->BaseAddress);
}
#else
XV_vscaler_Config *XV_vscaler_LookupConfig(UINTPTR BaseAddress) {
    XV_vscaler_Config *ConfigPtr = NULL;

    int Index;

    for (Index = (u32)0x0; XV_vscaler_ConfigTable[Index].Name != NULL; Index++) {
		if ((XV_vscaler_ConfigTable[Index].BaseAddress == BaseAddress) ||
		     !BaseAddress) {
			ConfigPtr = &XV_vscaler_ConfigTable[Index];
			break;
		}
    }

    return ConfigPtr;
}

int XV_vscaler_Initialize(XV_vscaler *InstancePtr, UINTPTR BaseAddress) {
    XV_vscaler_Config *ConfigPtr;

    Xil_AssertNonvoid(InstancePtr != NULL);

    ConfigPtr = XV_vscaler_LookupConfig(BaseAddress);
    if (ConfigPtr == NULL) {
        InstancePtr->IsReady = 0;
        return (XST_DEVICE_NOT_FOUND);
    }

    return XV_vscaler_CfgInitialize(InstancePtr,
                                    ConfigPtr,
                                    ConfigPtr->BaseAddress);
}
#endif
#endif
