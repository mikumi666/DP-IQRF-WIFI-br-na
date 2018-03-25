/*
 * fram.h
 *
 *  Created on: 19. 3. 2018
 *      Author: mirek
 */

#ifndef SRC__DRIVERS_FRAM_FRAM_H_
#define SRC__DRIVERS_FRAM_FRAM_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	FRAM_Event_TxComplete, FRAM_Event_RxComplete
}FRAM_Event_t;

void FRAM_Init(void (*FRAM_Event)(FRAM_Event_t));
void FRAM_Enable(void);
void FRAM_Disable(void);
bool FRAM_Write(uint32_t addr, void* data, uint16_t len);
bool FRAM_Read(uint32_t addr, void* data, uint16_t len);
bool FRAM_ReadStat();


#endif /* SRC__DRIVERS_FRAM_FRAM_H_ */
