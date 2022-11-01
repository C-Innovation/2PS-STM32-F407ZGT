/*
 * circular_buffer.h
 *
 *  Created on: 5 ���. 2020 �.
 *      Author: ivanov.y
 */

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include "main.h"

typedef uint8_t __type;
//#define __type			double

#define RAM_START_ADDRESS (uint32_t)(0x60000000 + 0x1000 )//FSMC_NORSRAM_EXTENDED_DEVICE 0x11C

typedef struct
{
  void (*Begin)(volatile uint32_t capacity);
  void (*End)();
	void (*PushBack)(__type item);
	void (*PushFront)(__type item);
	void (*PushBackArray)(__type *items, uint32_t len);
	void (*PopBack)();
	void (*PopFront)();
	void (*PopFrontArray)(uint32_t len);
	__type (*Front)();
	__type (*Back)();
	__type* (*FrontArray)(uint32_t len);
	__type (*ItemFrom)(uint32_t index);
	FlagStatus (*IsFull)();
	FlagStatus (*IsEmpty)();
	uint32_t (*Size)();
}Ci_CircularBufferTypeDef;

Ci_CircularBufferTypeDef ci_circular_buffer_new_instance();



#endif /* CIRCULAR_BUFFER_H_ */
