/*
 * batmon.c
 *
 *  Created on: 1. 4. 2018
 *      Author: mirek
 */
#include "batmon.h"

#include "MKL28Z7.h"
#include "pcc.h"

#include "lpi2c/lpi2c.h"

#define BATMON_I2C_ADDR				0xAA
#define BATMON_LPI2C_CLKFG_PCS		PCC_CKFG_PCS_VAL_SIRCLK

#define BATMON_PCC_LPIT				PCC_LPIT0
#define BATMON_LPIT					LPIT0

#define BATMON_LPIT_CLKCFG_PCS		PCC_CKFG_PCS_VAL_SOSCCLK
#define BATMON_LPIT_PRI				3
#define BATMON_LPIT_TVAL			1u
#define BATMON_LPIT_CHANNEL			0u
#define BATMON_LPIT_CH_MASK			(1ul << BATMON_LPIT_CHANNEL)

/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef enum
{
	BATMON_State_Deinit, BATMON_State_Disable, BATMON_State_Idle,
	BATMON_State_Bussy, BATMON_State_Running
}BATMON_State_t;

/****************************Prototypes*****************************/
/////////////////////////////////////////////////////////////////////

static void BATMON_LPI2C_callback(LPI2C_Event_t event);

static const LPI2C_Chann_t BATMON_LPI2C_Chann = LPI2C_Chann_LPI2C0;

/*****************************Variables*****************************/
/////////////////////////////////////////////////////////////////////

static BATMON_RegMap_t* RegMap;
static BATMON_RegMap_t tempRegMap;
static BATMON_State_t	state = BATMON_State_Deinit;

static void (*event)(void);

/****************************IRQ handlers***************************/
/////////////////////////////////////////////////////////////////////

void __attribute__ ((interrupt)) LPIT0_IRQHandler(void)
{
	if(BATMON_LPIT->MSR & BATMON_LPIT_CH_MASK)
	{
		BATMON_LPIT->MSR = BATMON_LPIT_CH_MASK;
		LPI2C_ReadData(BATMON_LPI2C_Chann, BATMON_I2C_ADDR, 0x02, &tempRegMap, sizeof(BATMON_RegMap_t));
	}
}

void BATMON_Init(BATMON_RegMap_t* BATMON_regMap, void (*BATMON_event)())
{
	if(state == BATMON_State_Deinit)
	{
		RegMap = BATMON_regMap;
		event = BATMON_event;

		NVIC_SetPriority(LPIT0_IRQn, BATMON_LPIT_PRI);
		NVIC_EnableIRQ(LPIT0_IRQn);

		state = BATMON_State_Disable;
	}
}

/*****************************functions*****************************/
/////////////////////////////////////////////////////////////////////

bool BATMON_Enable(void)
{
	bool retVar = false;
	if(state == BATMON_State_Disable)
	{
		BATMON_PCC_LPIT = PCC_CLKCFG_CGC_MASK | PCC_CLKCFG_PCS(BATMON_LPIT_CLKCFG_PCS);

		BATMON_LPIT->MCR = LPIT_MCR_M_CEN_MASK;
		BATMON_LPIT->MIER = LPIT_MIER_TIE0_MASK;

		LPI2C_Init(BATMON_LPI2C_Chann, BATMON_LPI2C_CLKFG_PCS, BATMON_LPI2C_callback);

		state = BATMON_State_Idle;
		retVar = true;
	}
	return retVar;
}

bool BATMON_Disable(void)
{
	bool retVar = false;
	if(state != BATMON_State_Disable || state != BATMON_State_Deinit)
	{


		state = BATMON_State_Disable;
		retVar = true;
	}
	return retVar;
}

bool BATMON_ReadState(void)
{
	bool retVar = false;
	if(state == BATMON_State_Idle)
	{
		LPI2C_ReadData(BATMON_LPI2C_Chann, BATMON_I2C_ADDR, 0x02, &tempRegMap, sizeof(BATMON_RegMap_t));

		state = BATMON_State_Bussy;
		retVar = true;
	}

	return retVar;
}

bool BATMON_StartReading(uint8_t period_s)
{
	bool retVar = false;
	if(state == BATMON_State_Idle)
	{
		BATMON_LPIT->MCR = LPIT_MCR_M_CEN_MASK;
		BATMON_LPIT->CHANNEL[BATMON_LPIT_CHANNEL].TVAL = period_s * 32768ul;
		BATMON_LPIT->MIER = BATMON_LPIT_CH_MASK;
		BATMON_LPIT->SETTEN = BATMON_LPIT_CH_MASK;

		state = BATMON_State_Running;
		retVar = true;
	}
	return retVar;
}

static void BATMON_LPI2C_callback(LPI2C_Event_t event)
{
	if(event == LPI2C_Event_RxComplete)
	{
		if(state == BATMON_State_Running)
		{
			*RegMap = tempRegMap;
			BATMON_Status.Voltage = tempRegMap.VOLT;
			BATMON_Status.Current = tempRegMap.INSTCURR;
			BATMON_Status.Temperature = tempRegMap.TEMP - 2832;
			BATMON_Status.SOC = (uint8_t)tempRegMap.SOC;
			BATMON_Status.SOH = (uint8_t)tempRegMap.SOH;
		}
		else if(state == BATMON_State_Bussy)
		{
			*RegMap = tempRegMap;
			BATMON_Status.Voltage = tempRegMap.VOLT;
			BATMON_Status.Current = tempRegMap.INSTCURR;
			BATMON_Status.Temperature = tempRegMap.TEMP - 2832;
			BATMON_Status.SOC = (uint8_t)tempRegMap.SOC;
			BATMON_Status.SOH = (uint8_t)tempRegMap.SOH;
			state = BATMON_State_Idle;
		}
		else
		{

		}
	}
}

