/*
 * fram.h
 *	FRAM: CY15B104Q
 *  Created on: 19. 3. 2018
 *      Author: mirek
 */

#ifndef FRAM_H_
#define FRAM_H_

#include <stdbool.h>
#include <stdint.h>

/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef enum
{
	FRAM_Event_TxComplete, FRAM_Event_RxComplete
}FRAM_Event_t;


/****************************prototypes*****************************/
/////////////////////////////////////////////////////////////////////

void FRAM_Init(void (*FRAM_Event)(FRAM_Event_t));
bool FRAM_Enable(void);
bool FRAM_Disable(void);
bool FRAM_Write(uint32_t addr, void* data, uint16_t len);
bool FRAM_Read(uint32_t addr, void* data, uint16_t len);
bool FRAM_ReadStat(uint8_t* data);


#endif /* FRAM_H_ */
