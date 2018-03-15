#include "LPUARTHandler.h"


/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef struct
{
	bool blocking;
	LPUART_Type* const regSet;
	const IRQn_Type IRQn;
	uint32_t* const PCC_reg;
	char buffRX[LPUART_RX_BUFF_LEN];
	char buffTX[LPUART_TX_BUFF_LEN];
	QueueHandle_t queueBuffRX;
	QueueHandle_t queueBuffTX;
	void (*event)(LPUARTHandler_Event_t event);

}LPUARTHandler_Instance_t;

/****************************Prototypes*****************************/
/////////////////////////////////////////////////////////////////////

static void LPUART_IRQHandler(LPUARTHandler_Chann_t LPUART_p);

/*****************************Variables*****************************/
/////////////////////////////////////////////////////////////////////

static LPUARTHandler_Instance_t LPUARTHandler_Instance[3] = {{ false, LPUART0, LPUART0_IRQn, (uint32_t*) &PCC_LPUART0},
															{ false, LPUART1, LPUART1_IRQn, (uint32_t*) &PCC_LPUART1},
															{ false, LPUART2, LPUART2_IRQn, (uint32_t*) &PCC_LPUART2}};
static const char LF = '\n';

/****************************IRQ handlers***************************/
/////////////////////////////////////////////////////////////////////

void __attribute__ ((interrupt)) LPUART0_IRQHandler(void)
{
	LPUART_IRQHandler(LPUARTHandler_Chann_LPUART0);
}

void __attribute__ ((interrupt)) LPUART1_IRQHandler(void)
{
	LPUART_IRQHandler(LPUARTHandler_Chann_LPUART1);
}

void __attribute__ ((interrupt)) LPUART2_IRQHandler(void)
{
	LPUART_IRQHandler(LPUARTHandler_Chann_LPUART2);
}

/*****************************functions*****************************/
/////////////////////////////////////////////////////////////////////

void LPUARTHandler_Init(LPUARTHandler_Chann_t channel, uint32_t LPUARTHandler_CLKCFG_PCS,
		uint32_t LPUARTHandler_BAUD_OSR, uint32_t LPUARTHandler_BAUD_SBR, void (*LPUARTHandler_Event)(LPUARTHandler_Event_t))
{
	LPUARTHandler_Instance_t* LPUART_p = &LPUARTHandler_Instance[channel];

	Queue_Init(&LPUART_p->queueBuffRX, LPUART_p->buffRX, LPUART_RX_BUFF_LEN, Queue_DataType_char);
	Queue_Init(&LPUART_p->queueBuffTX, LPUART_p->buffTX, LPUART_TX_BUFF_LEN, Queue_DataType_char);

	LPUART_p->event = LPUARTHandler_Event;

	*LPUART_p->PCC_reg = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(LPUARTHandler_CLKCFG_PCS);		// povoleni a vyber hodin

	LPUART_p->regSet->BAUD = LPUART_BAUD_OSR(LPUARTHandler_BAUD_OSR);		// Set OSR and SBR registers to set baudrate
	LPUART_p->regSet->BAUD |= LPUART_BAUD_SBR(LPUARTHandler_BAUD_SBR);

	LPUART_p->regSet->CTRL |= LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK;		// Enable tx and rx
	LPUART_p->regSet->CTRL |= LPUART_CTRL_RIE_MASK;								// Enable rx interrupt

	NVIC_SetPriority(LPUART_p->IRQn, LPUARTHANDLER_IRQ_PRI);					// Set LPUARTX IRQ priority
	NVIC_EnableIRQ(LPUART_p->IRQn);											// Enable LPUARTX IRQ
}

LPUARTHandler_Res_t LPUARTHandler_SendLine(LPUARTHandler_Chann_t channel, char data[])
{
	LPUARTHandler_Instance_t* LPUART_p = &LPUARTHandler_Instance[channel];
	uint16_t i = 0;
	LPUARTHandler_Res_t res;

	if(!LPUART_p->blocking)
	{
		LPUART_p->blocking = true;
		while (data[i] != '\0' && i < LPUART_TX_BUFF_LEN)
		{
			Queue_Enqueue(LPUART_p->queueBuffTX, &data[i]);
			i++;
		}

		Queue_Enqueue(LPUART_p->queueBuffTX, (void*)&LF);

		LPUART_p->blocking = false;
		LPUART_p->regSet->CTRL |= LPUART_CTRL_TIE_MASK;

		res = LPUARTHandler_Res_OK;
	}
	else
	{
		res = LPUARTHandler_Res_Blocked;
	}
	return res;
}

LPUARTHandler_Res_t LPUARTHandler_SendData(LPUARTHandler_Chann_t channel, void* data, uint16_t len)
{
	LPUARTHandler_Instance_t* LPUART_p = &LPUARTHandler_Instance[channel];
	uint16_t i;
	LPUARTHandler_Res_t res;

	if(!LPUART_p->blocking)
	{
		LPUART_p->blocking = true;

		for (i = 0; i < len; i++)
		{
			Queue_Enqueue(LPUART_p->queueBuffTX, &(((uint8_t *)data)[i]));
		}

		LPUART_p->blocking = false;
		LPUART_p->regSet->CTRL |= LPUART_CTRL_TIE_MASK;

		res = LPUARTHandler_Res_OK;
	}
	else
	{
		res = LPUARTHandler_Res_Blocked;
	}
	return res;
}

void LPURTHandler_ReadLine (LPUARTHandler_Chann_t channel, void* data, uint16_t maxLen, uint16_t recLen)
{
	LPUARTHandler_Instance_t* LPUART_p = &LPUARTHandler_Instance[channel];
	uint16_t i;
	char rxByte;

	if (!LPUART_p->blocking)
	{
		LPUART_p->blocking = true;

		if (Queue_getUsedCount(LPUART_p->queueBuffRX) <= maxLen)
		{
			for (i = 0; i < maxLen; i++)
			{
				if (Queue_Dequeue(LPUART_p->queueBuffRX, &rxByte) == Queue_Res_OK)
				{
					if (rxByte != LF)
					{
						((char*)data)[i] = rxByte;
					}
					else
					{
						break;
					}

				}
				else
				{
					break;
				}
			}
		}
		else
		{

		}

		LPUART_p->blocking = false;
	}
}


static void LPUART_IRQHandler(LPUARTHandler_Chann_t channel)
{
	LPUARTHandler_Instance_t* LPUART_p = &LPUARTHandler_Instance[channel];

	if (LPUART_p->regSet->STAT & LPUART_STAT_RDRF_MASK)
	{
		uint8_t rxByte = LPUART_p->regSet->DATA;
		if(rxByte != LF)
		{
			Queue_Enqueue(LPUART_p->queueBuffRX, &rxByte);
			LPUART_p->event(LPUARTHandler_Event_ReceiveByte);
		}
		else
		{
			LPUART_p->event(LPUARTHandler_Event_ReceiveLine);
		}
	}

	if ((LPUART_p->regSet->STAT & LPUART_STAT_TDRE_MASK) && (LPUART_p->regSet->CTRL & LPUART_CTRL_TIE_MASK))
	{
		uint8_t txByte;
		if(Queue_Dequeue(LPUART_p->queueBuffTX, &txByte) == Queue_Res_OK)
		{
			LPUART_p->regSet->DATA = txByte;
		}
		else
		{
			LPUART_p->regSet->CTRL &= ~LPUART_CTRL_TIE_MASK;
		}
	}
}

