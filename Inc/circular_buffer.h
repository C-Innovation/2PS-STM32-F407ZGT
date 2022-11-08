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
#define EXTERN_RAM_COL_BUF      7384
#define RAM_SIZE            (uint32_t)(0x100000)
#define RAM_START_ADDRESS   (uint32_t)(0x60000000)
#define RAM_END_ADDRESS     (uint32_t)(RAM_START_ADDRESS + RAM_SIZE)


#define RAM_CIRCULAR_BUFF_SIZE    RAM_SIZE
#define UART_BUFFER_SIZE          128
typedef enum
{
  EXTERN_RAM_OK = 0,
  ERROR_ETH_BUF_RAM,
  ERROR_SD_BUF_RAM,
  ERROR_EXTERN_RAM
} ExternRAM_Status;

typedef struct
{
  uint8_t lengh[2];
  uint8_t ID;
  uint8_t channel;
  uint8_t data[UART_BUFFER_SIZE];
} SaveDataUART;

typedef struct
{
  SaveDataUART data[1 * EXTERN_RAM_COL_BUF];
} SaveMasDataUART, *pSaveMasDataUART;

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

ExternRAM_Status ExternRAM_Test();
void ExternRAM_Erase();

#endif /* CIRCULAR_BUFFER_H_ */
