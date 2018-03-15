/*
 * USB.h
 *
 *  Created on: 19. 2. 2018
 *      Author: mirek
 */

#ifndef SRC__UTIL_USB_H_
#define SRC__UTIL_USB_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "USBkhci.h"
#include "USB_defs.h"

 /* USB module features */

 /* @brief KHCI module instance count */
 #define FSL_FEATURE_USB_KHCI_COUNT (1)
 /* @brief HOST mode enabled */
 #define FSL_FEATURE_USB_KHCI_HOST_ENABLED (1)
 /* @brief OTG mode enabled */
 #define FSL_FEATURE_USB_KHCI_OTG_ENABLED (1)
 /* @brief Size of the USB dedicated RAM */
 #define FSL_FEATURE_USB_KHCI_USB_RAM (2048)
 /* @brief Base address of the USB dedicated RAM */
 #define FSL_FEATURE_USB_KHCI_USB_RAM_BASE_ADDRESS (1074790400)
 /* @brief Has KEEP_ALIVE_CTRL register */
 #define FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED (0)
 /* @brief Has the Dynamic SOF threshold compare support */
 #define FSL_FEATURE_USB_KHCI_DYNAMIC_SOF_THRESHOLD_COMPARE_ENABLED (1)
 /* @brief Has the VBUS detect support */
 #define FSL_FEATURE_USB_KHCI_VBUS_DETECT_ENABLED (1)
 /* @brief Has the IRC48M module clock support */
 #define FSL_FEATURE_USB_KHCI_IRC48M_MODULE_CLOCK_ENABLED (0)
 /* @brief Number of endpoints supported */
 #define USBDEVICE_ENDPT_COUNT (16)

void USB_devInit(void (*USBdevice_Event)());



#endif /* SRC__UTIL_USB_H_ */
