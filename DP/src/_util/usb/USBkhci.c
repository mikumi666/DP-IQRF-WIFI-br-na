/*
 * USBkhci.c
 *
 *  Created on: 26. 2. 2018
 *      Author: mirek
 */
#include "USBkhci.h"

/*! @brief Set BDT control fields*/
#define USBKHCI_BDT_SET_CONTROL(bdt_base, ep, direction, odd, control)                \
    *(volatile uint32_t *)((bdt_base & 0xfffffe00U) | (((uint32_t)ep & 0x0fU) << 5U) | \
    		(((uint32_t)direction & 1U) << 4U) | (((uint32_t)odd & 1U) << 3U)) = control

/* typy */

typedef struct
{
	uint8_t* buff;
	uint32_t len;
	uint8_t code;
	bool isSetup;
}USBkhci_message_t;

/* proměné  */


__attribute__((aligned(512))) static uint8_t usbKhci_BDT_buffer[512U];
static uint32_t usbKhci_DMAalignBuff[((64 - 1) >> 2) + 1];

static USBkhci_state_t usbKhci_state = {.regSet = (USB_Type*)USB0_BASE,
										.dmaAlignBuff = (uint8_t *)(&usbKhci_DMAalignBuff[0]),
										.bdt = usbKhci_BDT_buffer};

/* prototypy */

static void USBkhci_ISRFcn_Reset();


static void USBkhci_devNotification(USBkhci_message_t* msg);
static USBdevice_status_t USBkhci_devResetNotification(USBkhci_message_t* msg);


/*IRQ*/



void __attribute__ ((interrupt)) USB0_IRQHandler(void)
{
	uint8_t intStat = usbKhci_state.regSet->ISTAT;

	if(intStat & USBkhci_interrupt_Reset)
	{
		USBkhci_ISRFcn_Reset();
	}
}

/* funkce  */

USBdevice_status_t USBkhci_devInit(void)
{
	usbKhci_state.regSet->ISTAT = 0xFFu;

	usbKhci_state.regSet->OTGCTL &= ~USB_OTGCTL_DPHIGH_MASK;
	usbKhci_state.regSet->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;

	usbKhci_state.regSet->BDTPAGE1 = (uint8_t)((((uint32_t)usbKhci_state.bdt) >> 8u) & 0xFFu);
	usbKhci_state.regSet->BDTPAGE2 = (uint8_t)((((uint32_t)usbKhci_state.bdt) >> 16u) & 0xFFu);
	usbKhci_state.regSet->BDTPAGE3 = (uint8_t)((((uint32_t)usbKhci_state.bdt) >> 24u) & 0xFFu);

	usbKhci_state.regSet->ERRSTAT = 0xFFu;
	usbKhci_state.regSet->CTL |= USB_CTL_ODDRST_MASK;
	usbKhci_state.regSet->ADDR = 0u;

	for (uint8_t i = 0; i < USBDEVICE_CONFIG_ENDPTS; i++)
	{
		USBKHCI_BDT_SET_CONTROL((uint32_t)usbKhci_state.bdt, i, USBDEVICE_OUT, 0u, 0u);
		USBKHCI_BDT_SET_CONTROL((uint32_t)usbKhci_state.bdt, i, USBDEVICE_OUT, 1u, 0u);
		USBKHCI_BDT_SET_CONTROL((uint32_t)usbKhci_state.bdt, i, USBDEVICE_IN, 0u, 0u);
		USBKHCI_BDT_SET_CONTROL((uint32_t)usbKhci_state.bdt, i, USBDEVICE_IN, 1u, 0u);

		usbKhci_state.endpointState[i*2 + USBDEVICE_OUT].stateUnion.state = 0u;
		usbKhci_state.endpointState[i*2 + USBDEVICE_IN].stateUnion.state = 0u;
		usbKhci_state.regSet->ENDPOINT[i].ENDPT = 0x00u;
	}

	usbKhci_state.regSet->CTL &= ~USB_CTL_ODDRST_MASK;

	usbKhci_state.regSet->ERREN = 0xFFu;

	usbKhci_state.regSet->INTEN = USBkhci_interrupt_Reset;

	usbKhci_state.isReseting = false;

	usbKhci_state.regSet->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;


	return USBdevice_status_Success;
}

USBdevice_status_t USBkhci_devControl(USBkhci_devControl_t type, void* param)
{
	USBdevice_status_t error = USBdevice_status_Error;
	switch (type)
	{
	case USBkhci_devControl_Run:
		usbKhci_state.regSet->USBCTRL = 0u;
		usbKhci_state.regSet->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
		usbKhci_state.regSet->CTL |= USB_CTL_USBENSOFEN_MASK;

		error = USBdevice_status_Success;
		break;
	case USBkhci_devControl_Stop:
		usbKhci_state.regSet->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;

		error = USBdevice_status_Success;
		break;
	case USBkhci_devControl_EndPtInit:

		break;
	case USBkhci_devControl_EndPtDeinit:

		break;
	case USBkhci_devControl_EndPtStall:

		break;
	case USBkhci_devControl_EndPtUnstall:

		break;
	case USBkhci_devControl_GetDevStatus:

		break;
	case USBkhci_devControl_GetEndPtStatus:

		break;
	case USBkhci_devControl_SetDevAddr:

		break;
	case USBkhci_devControl_GetSynchFrame:

		break;
	case USBkhci_devControl_SetDefStatus:

		break;
	case USBkhci_devControl_SetTestMode:

		break;
	default:
		break;
	}
}

static void USBkhci_ISRFcn_Reset(void)
{
	USBkhci_message_t msg;


	usbKhci_state.isReseting = true;
	usbKhci_state.regSet->ISTAT = USBkhci_interrupt_Reset;

	msg.buff = (uint8_t*)NULL;
	msg.code = USBkhci_devNotify_BusReset;
	msg.isSetup = false;
	msg.len = 0u;

	USBkhci_devNotification(&msg);
}

static void USBkhci_devNotification(USBkhci_message_t* msg)
{
	uint8_t endPt = msg->code & USBDEVICE_ENDPTS_NUM_MASK;
	USBdevice_status_t error = USBdevice_status_Error;

	if(msg->code == USBkhci_devNotify_BusReset)
	{
		usbKhci_state.isReseting = true;
		error = USBkhci_devResetNotification(msg);
	}
	else if (endPt < USBDEVICE_CONFIG_ENDPTS)
	{

	}
}

static USBdevice_status_t USBkhci_devResetNotification(USBkhci_message_t* msg)
{
	usbKhci_state.isReseting = true;
	USBkhci_devControl(USBkhci_devControl_SetDefStatus, NULL);

	//usbKhci_state.state = USBkhci_state_t;

	// zavolat callback po resetu
	usbKhci_state.isReseting = false;

}


