/*
 * queue.h
 *
 *  Created on: 11. 2. 2018
 *      Author: mirek
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

/******************************Defines******************************/
/////////////////////////////////////////////////////////////////////

#define QUEUE_MAXCOUNT			20


/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef void* QueueHandle_t;

typedef enum
{
	Queue_DataType_char,
	Queue_DataType_int8,
	Queue_DataType_uint8,
	Queue_DataType_int16,
	Queue_DataType_uint16,
	Queue_DataType_int32,
	Queue_DataType_uint32,
	Queue_DataType_float,
	Queue_DataType_int64,
	Queue_DataType_uint64,
	Queue_DataType_double

}Queue_DataType_t;

typedef enum
{
	Queue_Res_OK,
	Queue_Res_Full,
	Queue_Res_Empty,
	Queue_Res_Locked
}Queue_Res_t;

/****************************prototypes*****************************/
/////////////////////////////////////////////////////////////////////

void Queue_Init(QueueHandle_t* handle, void* buffer, uint16_t size, Queue_DataType_t type);
Queue_Res_t Queue_Enqueue(QueueHandle_t handle, void* element);
Queue_Res_t Queue_Dequeue(QueueHandle_t handle, void* element);
uint16_t Queue_getFreeCount(QueueHandle_t handle);
uint16_t Queue_getUsedCount(QueueHandle_t handle);

#endif /* _QUEUE_H_ */
