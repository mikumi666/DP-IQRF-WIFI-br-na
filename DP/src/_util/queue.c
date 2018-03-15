#include "queue.h"

/******************************Typedef******************************/
/////////////////////////////////////////////////////////////////////

typedef struct
{
	void* buffer;
	uint16_t size;
	uint16_t used;
	uint16_t start;
	bool mutex;
	Queue_DataType_t type;
}Queue_Instance_t;

/*****************************Variables*****************************/
/////////////////////////////////////////////////////////////////////

static Queue_Instance_t Queue_Instances[QUEUE_MAXCOUNT];
static uint8_t QueuesCount;

/*****************************functions*****************************/
/////////////////////////////////////////////////////////////////////

void Queue_Init(QueueHandle_t* handle, void* buffer, uint16_t size, Queue_DataType_t type)
{
	if (QueuesCount < QUEUE_MAXCOUNT)
	{
		Queue_Instance_t* queue_p = &Queue_Instances[QueuesCount];
		QueuesCount++;

		Queue_Instances[QueuesCount].buffer = buffer;
		Queue_Instances[QueuesCount].size = size;
		Queue_Instances[QueuesCount].used = 0;
		Queue_Instances[QueuesCount].start = 0;
		Queue_Instances[QueuesCount].mutex = false;
		Queue_Instances[QueuesCount].type = type;

		*handle = (QueueHandle_t)&Queue_Instances[QueuesCount];

	}
}

Queue_Res_t Queue_Enqueue(QueueHandle_t handle, void* element)
{
	Queue_Instance_t* queue_p = (Queue_Instance_t*)handle;
	Queue_Res_t res;

	if (!queue_p->mutex)
	{
		queue_p->mutex = true;

		if (queue_p->used < queue_p->size)
		{
			uint16_t nextPos = (queue_p->start + queue_p->used)%queue_p->size;
			switch (queue_p->type)
			{
			case Queue_DataType_char:
			case Queue_DataType_int8:
			case Queue_DataType_uint8:
				((uint8_t*)queue_p->buffer)[nextPos] = *(uint8_t*)element;
				break;
			case Queue_DataType_int16:
			case Queue_DataType_uint16:
				((uint16_t*)queue_p->buffer)[nextPos] = *(uint16_t*)element;
				break;
			case Queue_DataType_float:
			case Queue_DataType_int32:
			case Queue_DataType_uint32:
				((uint32_t*)queue_p->buffer)[nextPos] = *(uint32_t*)element;
				break;
			case Queue_DataType_double:
			case Queue_DataType_int64:
			case Queue_DataType_uint64:
				((uint64_t*)queue_p->buffer)[nextPos] = *(uint64_t*)element;
				break;
			}

			queue_p->used++;
			res = Queue_Res_OK;
		}
		else
		{
			res = Queue_Res_Full;
		}
		queue_p->mutex = false;
	}
	else
	{
		res = Queue_Res_Locked;
	}
	return res;
}

Queue_Res_t Queue_Dequeue(QueueHandle_t handle, void* item)
{
	Queue_Instance_t* queue_p = (Queue_Instance_t*)handle;
	Queue_Res_t res;

	if(!queue_p->mutex)
	{
		queue_p->mutex = true;

		if(queue_p->used > 0)
		{
			switch (queue_p->type)
			{
			case Queue_DataType_char:
			case Queue_DataType_int8:
			case Queue_DataType_uint8:
				*((uint8_t*)item) = ((uint8_t*)queue_p->buffer)[queue_p->start];
				break;
			case Queue_DataType_int16:
			case Queue_DataType_uint16:

				break;
			case Queue_DataType_float:
			case Queue_DataType_int32:
			case Queue_DataType_uint32:

				break;
			case Queue_DataType_double:
			case Queue_DataType_int64:
			case Queue_DataType_uint64:

				break;
			}
			queue_p->used -= 1;
			if (queue_p->used == 0)
			{
				queue_p->start = 0;
			}
			else
			{
				queue_p->start = (queue_p->start + (uint16_t)1);
				queue_p->start = queue_p->start % queue_p->size;
			}
			res = Queue_Res_OK;
		}
		else
		{
			res = Queue_Res_Empty;
		}
		queue_p->mutex = false;
	}
	else
	{
		res = Queue_Res_Locked;
	}
	return res;
}

uint16_t Queue_getUsedCount(QueueHandle_t handle)
{
	Queue_Instance_t* queue_p = (Queue_Instance_t*)handle;
	return queue_p->used;
}

uint16_t Queue_getFreeCount(QueueHandle_t handle)
{
	Queue_Instance_t* queue_p = (Queue_Instance_t*)handle;
	return (queue_p->size - queue_p->used);
}

