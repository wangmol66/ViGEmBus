/*
* Virtual Gamepad Emulation Framework - Windows kernel-mode bus driver
* Copyright (C) 2016-2018  Benjamin H�glinger-Stelzer
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include "trace.h"
#include <ntddk.h>
#include <wdf.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <initguid.h>
#include "ViGEmBusDriver.h"
#include <ViGEm/km/BusShared.h>
#include "Queue.h"
#include <usb.h>
#include <usbbusif.h>
#include "Context.h"
#include "Util.h"
#include "UsbPdo.h"
#include "Xusb.h"
#include "Ds4.h"
#include "Xgip.h"


#pragma region Macros

#define MAX_INSTANCE_ID_LEN             80
#define HID_LANGUAGE_ID_LENGTH          0x04

#define HID_REQUEST_GET_REPORT          0x01
#define HID_REQUEST_SET_REPORT          0x09
#define HID_REPORT_TYPE_FEATURE         0x03

#define VIGEM_POOL_TAG                  0x45476956 // "EGiV"
#define XUSB_POOL_TAG                   'BSUX'
#define DRIVERNAME                      "ViGEm: "
#define MAX_HARDWARE_ID_LENGTH          0xFF

#define ORC_PC_FREQUENCY_DIVIDER        1000
#define ORC_TIMER_START_DELAY           500 // ms
#define ORC_TIMER_PERIODIC_DUE_TIME     500 // ms
#define ORC_REQUEST_MAX_AGE             500 // ms

#pragma endregion

#pragma region Helpers

//
// Extracts the HID Report ID from the supplied class request.
//
#define HID_GET_REPORT_ID(_req_) ((_req_->Value) & 0xFF)

//
// Extracts the HID Report type from the supplied class request.
//
#define HID_GET_REPORT_TYPE(_req_) ((_req_->Value >> 8) & 0xFF)

//
// Some insane macro-magic =3
// 
#define P99_PROTECT(...) __VA_ARGS__
#define COPY_BYTE_ARRAY(_dst_, _bytes_)   do {BYTE b[] = _bytes_; \
                                            RtlCopyMemory(_dst_, b, RTL_NUMBER_OF_V1(b)); } while (0)

#pragma endregion


#pragma region WDF callback prototypes

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD Bus_EvtDeviceAdd;
EVT_WDF_DEVICE_FILE_CREATE Bus_DeviceFileCreate;
EVT_WDF_FILE_CLOSE Bus_FileClose;

EVT_WDF_CHILD_LIST_CREATE_DEVICE Bus_EvtDeviceListCreatePdo;

EVT_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COMPARE Bus_EvtChildListIdentificationDescriptionCompare;

EVT_WDF_DEVICE_PREPARE_HARDWARE Pdo_EvtDevicePrepareHardware;

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL Pdo_EvtIoInternalDeviceControl;

EVT_WDF_TIMER Xgip_SysInitTimerFunc;

EVT_WDF_OBJECT_CONTEXT_CLEANUP Bus_EvtDriverContextCleanup;

EVT_WDF_TIMER Bus_PlugInRequestCleanUpEvtTimerFunc;

#pragma endregion

#pragma region Bus enumeration-specific functions

NTSTATUS
Bus_PlugInDevice(
    _In_ WDFDEVICE Device,
    _In_ WDFREQUEST Request,
    _In_ BOOLEAN IsInternal,
    _Out_ size_t* Transferred
);

NTSTATUS
Bus_UnPlugDevice(
    _In_ WDFDEVICE Device,
    _In_ WDFREQUEST Request,
    _In_ BOOLEAN IsInternal,
    _Out_ size_t* Transferred
);

NTSTATUS
Bus_CreatePdo(
    _In_ WDFDEVICE Device,
    _In_ PWDFDEVICE_INIT ChildInit,
    _In_ PPDO_IDENTIFICATION_DESCRIPTION Description
);

NTSTATUS
Bus_QueueNotification(
    WDFDEVICE Device,
    ULONG SerialNo,
    WDFREQUEST Request
);

NTSTATUS
Bus_XgipSubmitReport(
    WDFDEVICE Device,
    ULONG SerialNo,
    PXGIP_SUBMIT_REPORT Report,
    _In_ BOOLEAN FromInterface
);

NTSTATUS
Bus_SubmitReport(
    WDFDEVICE Device,
    ULONG SerialNo,
    PVOID Report,
    _In_ BOOLEAN FromInterface
);

WDFDEVICE 
Bus_GetPdo(
    IN WDFDEVICE Device, 
    IN ULONG SerialNo);

VOID
Bus_PdoStageResult(
    _In_ PINTERFACE InterfaceHeader,
    _In_ VIGEM_PDO_STAGE Stage,
    _In_ ULONG Serial,
    _In_ NTSTATUS Status
);

#pragma endregion

