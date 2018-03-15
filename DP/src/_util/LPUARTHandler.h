/*
 * LPUARTHandler.h
 * MCU: MKL28Z512VLL7
 *
 *  Created on: 10. 2. 2018
 *      Author: mirek
 */

#ifndef _LPUARTHANDLER_H_
#define _LPUARTHANDLER_H_

#include <stdbool.h>
#include "MKL28Z7.h"
#include "queue.h"

/******************************Defines******************************/
/////////////////////////////////////////////////////////////////////

#define LPUARTHANDLER_IRQ_PRI			(2u)
#define LPUART_TX_BUFF_LEN				(64u)
#define LPUART_RX_BUFF_LEN				(64u)

/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef enum
{
	LPUARTHandler_Chann_LPUART0 = 0,
	LPUARTHandler_Chann_LPUART1 = 1,
	LPUARTHandler_Chann_LPUART2 = 2
}LPUARTHandler_Chann_t;

typedef enum
{
	LPUARTHandler_Res_OK = 0, LPUARTHandler_Res_Blocked
}LPUARTHandler_Res_t;

typedef enum
{
	LPUARTHandler_Event_ReceiveLine,
	LPUARTHandler_Event_ReceiveByte
}LPUARTHandler_Event_t;


/****************************prototypes*****************************/
/////////////////////////////////////////////////////////////////////

void LPUARTHandler_Init(LPUARTHandler_Chann_t channel, uint32_t LPUARTHandler_CLKCFG_PCS,
		uint32_t LPUARTHandler_BAUD_OSR, uint32_t LPUARTHandler_BAUD_SBR, void (*LPUARTHandler_Event)(LPUARTHandler_Event_t));
LPUARTHandler_Res_t LPUARTHandler_SendLine(LPUARTHandler_Chann_t channel, char data[]);
LPUARTHandler_Res_t LPUARTHandler_SendData(LPUARTHandler_Chann_t channel, void* data, uint16_t len);
void LPURTHandler_ReadLine (LPUARTHandler_Chann_t channel, void* data, uint16_t maxLen, uint16_t recLen);

#endif /* _LPUARTHANDLER_H_ */
