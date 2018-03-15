 /*
 * USB.c
 *
 *  Created on: 19. 2. 2018
 *      Author: mirek
 */
#include "USB.h"
//#include "usb_device_khci.h"


typedef struct
{
	void (*callbackFcn)();
	void* callbackParam;
	bool isBusy;
}USBdevice_endpoint_t;

typedef struct
{

}USBdevice_controller_type_t;

typedef struct
{
	//USBdevice_status_t (*deviceInit)(uint8_t controllerId, void* handle, void** controllerHandle);
	USBdevice_status_t (*deviceInit)();
	//USBdevice_status_t (*deviceDeinit)(uint8_t controllerId, void* controllerHandle);
	//USBdevice_status_t (*deviceSend)(void* controllerHandle, uint8_t endpointAddr, uint8_t* buffer, uint32_t length);
	//USBdevice_status_t (*deviceRecv)(void* controllerHandle, uint8_t endpointAddr, uint8_t* buffer, uint32_t length);
	//USBdevice_status_t (*deviceCancel)(void* controllerHandle, uint8_t endpointAddr);
	//USBdevice_status_t (*deviceControl)(void* controllerHandle, USBdevice_controller_type_t command, void* param);
}USBdevice_controller_interface_t;

typedef struct
{
	const USBdevice_controller_interface_t* controllerInterface;
	const struct
	{
		USBdevice_status_t (*deviceInit)();
	}ctrlInterface;

	const IRQn_Type IRQn;
	USBdevice_endpoint_t endPoint[USBDEVICE_CONFIG_ENDPTS << 1u];
	void  (*USBdevice_Event)();
}USBdevice_t;

/* VARs*/

/*static USBdevice_t usbDevice = {.controllerInterface = {USB_DeviceKhciInit, USB_DeviceKhciDeinit, USB_DeviceKhciSend,
							USB_DeviceKhciRecv, USB_DeviceKhciCancel, USB_DeviceKhciControl}};*/
//static const USBdevice_controller_interface_t aaa = { .deviceInit = USBkhci_Init};
static USBdevice_t usbDevice;
//= {.ctrlInterface = USBkhci_Init, .IRQn = USB0_IRQn};
//= {.controllerInterface.controllerInterface = USBkhci_Init};

/* Funkce */

void USB_devInit(void (*USBdevice_Event)())
{
	uint8_t i;
	/*Přidat ** Vypni přerušení*/

	usbDevice.USBdevice_Event = USBdevice_Event;

	/*Přidat ** Zapni přerušení*/

	for(i = 0; i < (USBDEVICE_CONFIG_ENDPTS * 2u); i++)
	{
		usbDevice.endPoint[i].callbackFcn = NULL;
		usbDevice.endPoint[i].callbackParam = NULL;
		usbDevice.endPoint[i].isBusy = false;
	}
	usbDevice.ctrlInterface.deviceInit();

	NVIC_SetPriority(usbDevice.IRQn, USBDEVICE_IRQ_PRI);


}

