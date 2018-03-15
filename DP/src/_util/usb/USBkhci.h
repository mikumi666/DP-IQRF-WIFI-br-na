/*
 * USBkhci.h
 *
 *  Created on: 26. 2. 2018
 *      Author: mirek
 */

#ifndef SRC__UTIL_USB_USBKHCI_H_
#define SRC__UTIL_USB_USBKHCI_H_

#include <stdbool.h>
#include "MKL28Z7.h"
#include "USB.h"
#include "USB_defs.h"

typedef struct
{
	uint8_t* transfBuff;
	uint32_t transfLen;
	uint32_t transfDone;
	union
	{
		uint32_t state;
		struct
		{
			uint32_t maxPacketSize : 10u;
			uint32_t stalled : 1u;
			uint32_t data0 : 1u;
			uint32_t bdtOdd : 1u;
			uint32_t dmaAlign : 1u;
			uint32_t transferring : 1u;
			uint32_t zlt : 1u;
		}stateBitField;
	}stateUnion;
}USBkhci_endpointState_t;

typedef enum
{
	USBkhci_devState_Configured = 0u,
	USBkhci_devState_Address,
	USBkhci_devState_Default,
	USBkhci_devState_Addresing,
	USBkhci_devState_TestMode
}USBkhci_devState_t;

typedef struct
{
	uint8_t devAddr;
	USBkhci_devState_t state;
	bool isReseting;
	USBkhci_endpointState_t endpointState[USBDEVICE_CONFIG_ENDPTS * 2];
	USB_Type* const regSet;
	uint8_t* const dmaAlignBuff;
	uint8_t* const bdt;

}USBkhci_state_t;

typedef enum _USBkhci_interrupt_t
{
	USBkhci_interrupt_Reset = 0x01u,
	USBkhci_interrupt_Error = 0x02u,
	USBkhci_interrupt_SoftToken = 0x04u,
	USBkhci_interrupt_TokenDone = 0x08u,
	USBkhci_interrupt_Sleep = 0x10u,
	USBkhci_interrupt_Resume = 0x20u,
	USBkhci_interrupt_Attach = 0x40u,
	USBkhci_interrupt_Stall = 0x80u
}USBkhci_interrupt_t;


typedef enum
{
	USBkhci_devNotify_BusReset = 0x10u,
	USBkhci_devNotify_Suspend,
	USBkhci_devNotify_Resume,
	USBkhci_devNotify_LPMSleep,
	USBkhci_devNotify_LPResume,
	USBkhci_devNotify_Error,
	USBkhci_devNotify_Detach,
	USBkhci_devNotify_Attach
}USBkhci_devNotify_t;

typedef enum
{
	USBkhci_devControl_Run = 0u,
	USBkhci_devControl_Stop,
	USBkhci_devControl_EndPtInit,
	USBkhci_devControl_EndPtDeinit,
	USBkhci_devControl_EndPtStall,
	USBkhci_devControl_EndPtUnstall,
	USBkhci_devControl_GetDevStatus,
	USBkhci_devControl_GetEndPtStatus,
	USBkhci_devControl_SetDevAddr,
	USBkhci_devControl_GetSynchFrame,
	USBkhci_devControl_Resume,
	USBkhci_devControl_SleepResume,
	USBkhci_devControl_Suspend,
	USBkhci_devControl_Sleep,
	USBkhci_devControl_SetDefStatus,
	USBkhci_devControl_GetSpeed,
	USBkhci_devControl_GetOtgStatus,
	USBkhci_devControl_SetOtgStatus,
	USBkhci_devControl_SetTestMode,
	USBkhci_devControl_RemoteWakeUp
}USBkhci_devControl_t;



USBdevice_status_t USBkhci_devInit(void);
USBdevice_status_t USBkhci_devControl(USBkhci_devControl_t type, void* param);


#endif /* SRC__UTIL_USB_USBKHCI_H_ */
