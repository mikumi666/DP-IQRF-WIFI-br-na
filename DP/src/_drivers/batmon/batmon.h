/*
 * batmon.h
 *
 *  Created on: 1. 4. 2018
 *      Author: mirek
 */

#ifndef SRC__DRIVERS_BATMON_BATMON_H_
#define SRC__DRIVERS_BATMON_BATMON_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	int16_t AR;					/*  0x02/0x03	AtRate 				*/
	uint16_t ARTTE;				/*  0x04/0x05	AtRateTimeToEmpty	*/
	uint16_t TEMP;				/*  0x06/0x07	Temperature  		*/
	uint16_t VOLT;				/*  0x08/0x09	Voltage  			*/
	uint16_t FLAGS;				/*  0x0A/0x0B	BatteryStatus  		*/
	int16_t INSTCURR;			/*  0x0C/0x0D	Current 			*/
	uint8_t RESERVED_0[2];		/*	0x0E/0x0F	RESERVED			*/
	uint16_t RM;				/*  0x10/0x11	RemainingCapacity 	*/
	uint16_t FCC;				/*  0x12/0x13	FullChargeCapacity	*/
	int16_t AI;					/*  0x14/0x15	AverageCurrent		*/
	uint16_t TTE;				/*  0x16/0x17	AverageTimeToEmpty	*/
	uint16_t TTF;				/*  0x18/0x19	AverageTimeToFull	*/
	int16_t SI;					/*  0x1A/0x1B	StandbyCurrent		*/
	uint16_t STTE;				/*  0x1C/0x1D	StandbyTimeToEmpty	*/
	int16_t MLI;				/*  0x1E/0x1F	MaxLoadCurrent		*/
	uint16_t MLTTE;				/*  0x20/0x21	MaxLoadTimeToEmpty	*/
	int16_t AP;					/*  0x22/0x23	AveragePower		*/
	uint8_t RESERVED_1[4];		/*  0x24/0x27	RESERVED			*/
	uint16_t INT_TEMP;			/*  0x28/0x29	InternalTemperature	*/
	uint16_t CC;				/*  0x2A/0x2B	CycleCount			*/
	uint16_t SOC;				/*  0x2C/0x2D	StateOfCharge		*/
	uint16_t SOH;				/*  0x2E/0x2F	StateOfHealth		*/
	uint16_t CV;				/*  0x30/0x31	ChargingVoltage		*/
	uint16_t CI;				/*  0x32/0x33	ChargingCurrent		*/
}BATMON_RegMap_t;

typedef struct
{
	int16_t	Temperature;		/*	Temperature [°0,1°C]			*/
	uint16_t Voltage;			/*	Voltage [1mV]					*/
	int16_t Current;			/*	Current [1mA]					*/
	uint8_t SOC;				/*	State of charge [%]				*/
	uint8_t	SOH;				/*	State of health [%]				*/
}BATMON_Status_t;

static BATMON_Status_t BATMON_Status;

void BATMON_Init(BATMON_RegMap_t* BATMON_regMap, void (*BATMON_event)());
bool BATMON_Enable(void);
bool BATMON_ReadState(void);
bool BATMON_StartReading(uint8_t period_s);


#endif /* SRC__DRIVERS_BATMON_BATMON_H_ */
