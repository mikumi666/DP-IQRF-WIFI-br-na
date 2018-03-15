/*
 * wifi_rn131.c
 *
 *  Created on: 11. 3. 2018
 *      Author: mirek
 */

#include "wifi_rn131.h"

static const LPUARTHandler_Chann_t UART_chann = LPUARTHandler_Chann_LPUART2;
static void UART_callback(LPUARTHandler_Event_t event);

static uint8_t buff[100];

void rn131_Init(void)
{
	LPUARTHandler_Init(UART_chann, PCC_CKFG_PCS_VAL_SIRCLK, 32, 52, (void*)UART_callback);

}

void UART_callback(LPUARTHandler_Event_t event)
{
	if(event == LPUARTHandler_Event_ReceiveByte)
	{

	}
	if(event == LPUARTHandler_Event_ReceiveLine)
	{
		LPURTHandler_ReadLine(UART_chann, (void*)buff, 100, 5);
		if(buff[0] == 'C' && buff[1] == 'M')
		{
			LPUARTHandler_SendLine(UART_chann, "scan");
		}
	}
}

void rn131_test(void)
{
	char str[] = "$$$\r";
	uint32_t aa = GPIOD->PDIR;
	LPUARTHandler_SendData(UART_chann, (void*)str, 4);
}
