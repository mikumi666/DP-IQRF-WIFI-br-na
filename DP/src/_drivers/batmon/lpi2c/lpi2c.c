/*
 * i2c.c
 *
 *  Created on: 30. 3. 2018
 *      Author: mirek
 */

#include "lpi2c.h"
#include "lpi2c_prv.h"

#include <stdbool.h>
#include <stddef.h>

#include "MKL28Z7.h"


// MCR - MEN bit - povolení master režimu
// MSR - statusy - vlajky
// MIER - povolení různých přerušení (TDIE, RDIE, NDIE, ...)
// MCFGR1 - PRESCALE - nastavení děličky pro hodiny
// MCCR0 a MCCR1 - nastavení zpoždění pro různé blbosti
// MFCR - nastavení watermarku pro FIFA
// MTDR - registr pro odesílání - data + command
// MRDR - přijímací registr


/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef enum
{
	LPI2C_State_Idle, LPI2C_State_TX, LPI2C_State_RX, LPI2C_State_Error
}LPI2C_State_t;

typedef struct
{
	LPI2C_Type* const regSet;
	const IRQn_Type IRQn;
	uint32_t* const PCC_reg;
	const uint8_t clk_prescale;
	LPI2C_State_t state;
	void (*event)(LPI2C_Event_t event);

	uint8_t* rxBuff;
	uint16_t rxRecSize;
	uint16_t rxSendSize;
	uint16_t rxBuffSize;

	uint8_t* txBuff;
	uint16_t txSendSize;
	uint16_t txBuffSize;

}LPI2C_Instance_t;

/****************************Prototypes*****************************/
/////////////////////////////////////////////////////////////////////

static void LPI2C_IRQHandler(LPI2C_Chann_t channel);

/*****************************Variables*****************************/
/////////////////////////////////////////////////////////////////////

static LPI2C_Instance_t LPI2C_Instance[3] = {{ LPI2C0, LPI2C0_IRQn, &PCC_LPI2C0, LPI2C0_MCFGR1_PRESCALE },
											{  LPI2C1, LPI2C1_IRQn, &PCC_LPI2C1, LPI2C1_MCFGR1_PRESCALE },
											{  LPI2C2, LPI2C2_IRQn, &PCC_LPI2C2, LPI2C2_MCFGR1_PRESCALE }};

/****************************IRQ handlers***************************/
/////////////////////////////////////////////////////////////////////

void __attribute__ ((interrupt)) LPI2C0_IRQHandler(void)
{
	LPI2C_IRQHandler(LPI2C_Chann_LPI2C0);
}

void __attribute__ ((interrupt)) LPI2C1_IRQHandler(void)
{
	LPI2C_IRQHandler(LPI2C_Chann_LPI2C1);
}

void __attribute__ ((interrupt)) LPI2C2_IRQHandler(void)
{
	LPI2C_IRQHandler(LPI2C_Chann_LPI2C2);
}

/*****************************functions*****************************/
/////////////////////////////////////////////////////////////////////

void LPI2C_Init(LPI2C_Chann_t channel, uint32_t LPI2C_CLKCFG_PCS, void (*event)(LPI2C_Event_t))
{
	LPI2C_Instance_t* LPI2C_p = &LPI2C_Instance[channel];

	LPI2C_p->state = LPI2C_State_Idle;

	LPI2C_p->event = event;

	*LPI2C_p->PCC_reg = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(LPI2C_CLKCFG_PCS);

	LPI2C_p->regSet->MCR = LPI2C_MCR_RST_MASK;
	LPI2C_p->regSet->MCR = 0;

	LPI2C_p->regSet->MCFGR0 = 2;
	LPI2C_p->regSet->MCFGR1 = 0b001;// | (1u << 8); //LPI2C_p->clk_prescale;

	LPI2C_p->regSet->MFCR = LPI2C_MFCR_RXWATER(0) | LPI2C_MFCR_TXWATER(3);

	//LPI2C_p->regSet->MCCR0 = 0x09131326;
	//LPI2C_p->regSet->MCCR1 = 0x09131326;
	LPI2C_p->regSet->MCCR0 = LPI2C_MCCR0_DATAVD(5) | LPI2C_MCCR0_SETHOLD(19) | LPI2C_MCCR0_CLKHI(19) | LPI2C_MCCR0_CLKLO(19);
	//LPI2C_p->regSet->MCCR0 = LPI2C_MCCR0_DATAVD(62) | LPI2C_MCCR0_SETHOLD(62) | LPI2C_MCCR0_CLKHI(63) | LPI2C_MCCR0_CLKLO(63);

	LPI2C_p->regSet->MCFGR1 |= LPI2C_MCFGR1_AUTOSTOP_MASK;

	LPI2C_p->regSet->MCR = LPI2C_MCR_MEN_MASK;

	//LPI2C_p->regSet->MIER = LPI2C_MIER_TDIE_MASK | LPI2C_MIER_NDIE_MASK;

	NVIC_SetPriority(LPI2C_p->IRQn, LPI2C_IRQ_PRI);
	NVIC_EnableIRQ(LPI2C_p->IRQn);
}

bool LPI2C_ReadData(LPI2C_Chann_t chann, uint8_t addr, uint8_t reg, void* buff, uint16_t len)
{
	LPI2C_Instance_t* LPI2C_p = & LPI2C_Instance[chann];
	bool retVal = false;

	if(LPI2C_p->state == LPI2C_State_Idle)
	{
		LPI2C_p->state = LPI2C_State_RX;
		LPI2C_p->rxBuff = (uint8_t*)buff;
		LPI2C_p->rxBuffSize = len-1;
		LPI2C_p->rxRecSize = 0;
		LPI2C_p->rxSendSize = 0;

		//LPI2C_p->regSet->MCR |= LPI2C_MCR_RRF_MASK | LPI2C_MCR_RTF_MASK;

		LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(LPI2C_MTDR_CMD_START_NACK) | LPI2C_MTDR_DATA(addr);
		LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(LPI2C_MTDR_CMD_TX) | LPI2C_MTDR_DATA(reg);
		LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(LPI2C_MTDR_CMD_START) | LPI2C_MTDR_DATA(addr+1);
		LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(LPI2C_MTDR_CMD_RX);


		LPI2C_p->regSet->MIER = LPI2C_MIER_TDIE_MASK | LPI2C_MIER_RDIE_MASK | LPI2C_MIER_NDIE_MASK;

	}

	return retVal;
}

void LPI2C_Send()
{
	LPI2C_Instance_t* LPI2C_p = &LPI2C_Instance[0];
	/*LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(0xAA);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0x00);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0x01);
	for(int i = 0; i < 1000; i++);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0x00);
	LPI2C_p->regSet->MCFGR1 |= (1u << 8);

	for(int i = 0; i < 5000; i++);
	LPI2C_p->regSet->MSR = 0;
	for(int i = 0; i < 5000; i++);*/

	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b111) | LPI2C_MTDR_DATA(0xAA);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0x08);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b110) | LPI2C_MTDR_DATA(0xAB);
	for(int i = 0; i < 1000; i++);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b001);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b001);
	LPI2C_p->regSet->MCFGR1 |= (1u << 8);
	//LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b101) | LPI2C_MTDR_DATA(0xAB);
	//LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b001);*/
	/*for(int i = 0; i < 1500; i++);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0x08);
	for(int i = 0; i < 1500; i++);
	LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(0b001);*/
}

static void LPI2C_IRQHandler(LPI2C_Chann_t channel)
{
	LPI2C_Instance_t* LPI2C_p = &LPI2C_Instance[channel];

	if((LPI2C_p->regSet->MIER & LPI2C_MIER_TDIE_MASK) && (LPI2C_p->regSet->MSR & LPI2C_MSR_TDF_MASK))
	{
		if(LPI2C_p->state == LPI2C_State_TX)
		{
			LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(LPI2C_MTDR_CMD_TX)
									| LPI2C_MTDR_DATA(LPI2C_p->txBuff[LPI2C_p->txSendSize++]);
			if(LPI2C_p->txSendSize == LPI2C_p->txBuffSize)
			{
				LPI2C_p->regSet->MIER &= ~LPI2C_MIER_TDIE_MASK;
				if(LPI2C_p->event != NULL)
				{
					LPI2C_p->event(LPI2C_Event_TxComplete);
				}
			}
		}
		else if(LPI2C_p->state == LPI2C_State_RX)
		{
			if(LPI2C_p->rxSendSize < LPI2C_p->rxBuffSize)
			{
				LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(LPI2C_MTDR_CMD_RX);
				LPI2C_p->rxSendSize++;
			}
			else
			{
				//LPI2C_p->regSet->MTDR = LPI2C_MTDR_CMD(LPI2C_MTDR_CMD_STOP);
				LPI2C_p->regSet->MIER &= ~LPI2C_MIER_TDIE_MASK;
			}
			/*if(++LPI2C_p->rxSendSize == LPI2C_p->rxBuffSize)
			{
			}*/
		}
		else
		{
			LPI2C_p->regSet->MIER &= ~LPI2C_MIER_TDIE_MASK;
		}
	}
	if((LPI2C_p->regSet->MIER & LPI2C_MIER_RDIE_MASK) && (LPI2C_p->regSet->MSR & LPI2C_MSR_RDF_MASK))
	{
		if(LPI2C_p->state & LPI2C_State_RX)
		{
			LPI2C_p->rxBuff[LPI2C_p->rxRecSize++] = (uint8_t)LPI2C_p->regSet->MRDR;
			if(LPI2C_p->rxRecSize == LPI2C_p->rxBuffSize)
			{
				LPI2C_p->regSet->MIER &= ~LPI2C_MIER_RDIE_MASK;

				LPI2C_p->state = LPI2C_State_Idle;
				if(LPI2C_p->event != NULL)
				{
					LPI2C_p->event(LPI2C_Event_RxComplete);
				}
			}
		}
		else
		{
			LPI2C_p->regSet->MIER &= ~LPI2C_MIER_RDIE_MASK;
		}
	}
	if((LPI2C_p->regSet->MIER & LPI2C_MIER_NDIE_MASK) && (LPI2C_p->regSet->MSR & LPI2C_MSR_NDF_MASK))
	{
		LPI2C_p->regSet->MSR |= LPI2C_MSR_NDF_MASK;
		LPI2C_p->event(LPI2C_Event_Error);
	}

}
