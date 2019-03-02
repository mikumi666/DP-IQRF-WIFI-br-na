/*
 * i2c.h
 *
 *  Created on: 30. 3. 2018
 *      Author: mirek
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stdbool.h>

#define LPI2C_IRQ_PRI					(2u)

#define LPI2C0_MCFGR1_PRESCALE			(0b001)
#define LPI2C1_MCFGR1_PRESCALE			(0b000)
#define LPI2C2_MCFGR1_PRESCALE			(0b000)


/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef enum
{
	LPI2C_Chann_LPI2C0 = 0,
	LPI2C_Chann_LPI2C1 = 1,
	LPI2C_Chann_LPI2C2 = 2
}LPI2C_Chann_t;

typedef enum
{
	LPI2C_Event_TxComplete, LPI2C_Event_RxComplete, LPI2C_Event_Error
}LPI2C_Event_t;

void LPI2C_Init(LPI2C_Chann_t channel, uint32_t LPI2C_CLKCFG_PCS, void (*event)(LPI2C_Event_t));
bool LPI2C_ReadData(LPI2C_Chann_t chann, uint8_t addr, uint8_t reg, void* buff, uint16_t len);
void LPI2C_Send();

#endif /* I2C_H_ */
