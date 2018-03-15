/*
 * USB_defs.h
 *
 *  Created on: 1. 3. 2018
 *      Author: mirek
 */

#ifndef SRC__UTIL_USB_USB_DEFS_H_
#define SRC__UTIL_USB_USB_DEFS_H_

#define USBDEVICE_CONFIG_ENDPTS 		(4u)

/* USB  standard descriptor transfer direction (cannot change the value because iTD use the value directly) */
#define USBDEVICE_OUT 					(0u)
#define USBDEVICE_IN 					(1u)

#define USBDEVICE_IRQ_PRI				(3u)

#define USBDEVICE_ENDPTS_NUM_MASK		(4u)

typedef enum
{
	USBdevice_status_Success = 0u,
	USBdevice_status_Error
}USBdevice_status_t;



#endif /* SRC__UTIL_USB_USB_DEFS_H_ */
