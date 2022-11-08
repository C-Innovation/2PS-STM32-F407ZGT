/*
 * circular_buffer.c
 *
 *  Created on: 5 ���. 2020 �.
 *      Author: ivanov.y
 */

#include "circular_buffer.h"

static volatile uint32_t __start = 0;
static volatile uint32_t __end = 0;
static volatile uint32_t __size = 0;
//static volatile __type *__buffer;//[__capacity];

static void End();
static void Begin(volatile uint32_t capacity);
static void PushBack(__type item);
static void PushFront(__type item);
static void PushBackArray(__type *items, uint32_t len);
static void PopBack();
static void PopFront();
static void PopFrontArray(uint32_t len);

static __type Front();
static __type Back();
static __type* FrontArray(uint32_t len);
static __type ItemFrom(uint32_t index);

static FlagStatus IsFull();
static FlagStatus IsEmpty();

static uint32_t Size();
static uint32_t _increment(volatile uint32_t index);
static uint32_t _decrement(volatile uint32_t index);

static volatile uint32_t __capacity;

static FlagStatus _check_init();

static void  pReadBytes(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pDstBuffer, uint32_t BufferSize);
static void pWriteBytes(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pSrcBuffer, uint32_t BufferSize);

extern SRAM_HandleTypeDef hsram1;

static const char *TAG = "CI_CIRCULAR_BUFFER";

Ci_CircularBufferTypeDef ci_circular_buffer_new_instance()
{
  Ci_CircularBufferTypeDef cBuff =
  {
      Begin,
      End,
      PushBack,
      PushFront,
      PushBackArray,
      PopBack,
      PopFront,
      PopFrontArray,
      Front,
      Back,
      FrontArray,
      ItemFrom,
      IsFull,
      IsEmpty,
      Size
  };
  return cBuff;
}

static void Begin(volatile uint32_t capacity)
{
  __capacity = capacity;
  //__buffer = (__type *)malloc(capacity);
//  if(__buffer == NULL)
//    printf("Initialize ERROR! __buffer = NULL...");
}

static void End()
{
  if(!_check_init())
    return;
  //free(__buffer);
}

static void PushBack(__type item)
{
  uint32_t pAdr = __end + RAM_START_ADDRESS;
	if (IsFull())
	{

	  printf("Buffer is full!");
	}
	else
	{
		//__buffer[__end] = item;
	  pWriteBytes(&hsram1, pAdr, &item, 1);
		__end = _increment(__end);
		__size++;
	}
}



static void PushFront(__type item)
{
  uint32_t pAdr = __start + RAM_START_ADDRESS;
	if (IsFull())
	{

	  printf("Buffer is full!");
	}
	else
	{
		__start = _decrement(__start);
		//__buffer[__start] = item;
		pWriteBytes(&hsram1, pAdr, &item, 1);
		__size++;
	}
}

static void PushBackArray(__type *items, uint32_t len)
{
  uint32_t pAdr = __end + RAM_START_ADDRESS;
  if(len + __size >= __capacity){
    printf("Size is too large!");
    return;
  }
  if (IsFull())
  {

    printf("Buffer is full!");
  }
  else
  {
    if((len + __end) >= __capacity)
    {
      //memcpy(&__buffer[__end], &items[0], (__capacity - __end));
      pWriteBytes(&hsram1, pAdr, &items[0], (__capacity - __end));
      //memcpy(&__buffer[0], &items[(__capacity - __end)], (len - (__capacity - __end)));
      pAdr = RAM_START_ADDRESS;
      pWriteBytes(&hsram1, pAdr, &items[(__capacity - __end)], (len - (__capacity - __end)));
      __end = (len - (__capacity - __end));
      __size += len;
    }
    else
    {
     // memcpy(&__buffer[__end], items, len);
      pWriteBytes(&hsram1, pAdr, &items[0], len);
      __end += len;
      __size += len;
    }

  }
}


static void PopBack()
{
  if(!_check_init())
    return;
	if(IsEmpty())
	{
	  printf("Buffer is empty!");
		return;
	}
	__end = _decrement(__end);
	//__buffer[__end] = 0;
	__size--;
}

static void PopFront()
{

  if(IsEmpty())
  {
    printf("Buffer is empty!");
    return;
  }
	//__buffer[__start] = 0;
	__start = _increment(__start);
	__size--;
}

static void PopFrontArray(uint32_t len)
{

  if(IsEmpty())
  {
    printf("Buffer is empty!");
    return;
  }
  if(len > __size){
    printf("Size is too large!");
    return;
  }

  if((__start + len) >= __capacity)
  {
    __start = (len - (__capacity - __start));
  }
  else
  {
    __start += len;
  }

  __size -= len;
}


static __type Front()
{
  uint32_t pAdr = __start + RAM_START_ADDRESS;
  if(IsEmpty())
  {
    printf("Buffer is empty!");
    return 0;
  }
  __type out = 0;
  pReadBytes(&hsram1, pAdr, &out, 1);
	return out;
}

static __type Back()
{
  uint32_t pAdr = ((__end != 0 ? __end : __capacity) - 1) + RAM_START_ADDRESS;
  if(!_check_init())
    return 0;
  if(IsEmpty())
  {
    printf("Buffer is empty!");
    return 0;
  }
  __type out = 0;
   pReadBytes(&hsram1, pAdr, &out, 1);
	return out;
}

static __type* FrontArray(uint32_t len)
{
  uint32_t pAdr = __start + RAM_START_ADDRESS;
  if(IsEmpty())
  {
    printf("Buffer is empty!");
    return NULL;
  }
  if(len > __size)
  {
    printf("Size is too large!");
    return NULL;
  }

  __type* outBuf = (__type*)malloc(len);
  if((__start + len) >= __capacity)
  {
    //memcpy(&outBuf[0], &__buffer[__start], (__capacity - __start));
    pReadBytes(&hsram1, pAdr, &outBuf[0], (__capacity - __start));
    //memcpy(&outBuf[(__capacity - __start)], &__buffer[0], (len - (__capacity - __start)));
    pAdr = RAM_START_ADDRESS;
    pReadBytes(&hsram1, pAdr, &outBuf[(__capacity - __start)], (len - (__capacity - __start)));
  }
  else
  {
    //memcpy(outBuf, &__buffer[__start], len);

    pReadBytes(&hsram1, pAdr, outBuf, len);
  }
  return outBuf;
}


static __type ItemFrom(uint32_t index)
{

  if(IsEmpty())
  {
    printf("Buffer is empty!");
    return 0;
  }

  if(index > (__size - 1))
  {
    printf("Buffer size is too small!");
    return 0;
  }
  uint32_t pAdr = (((index + __start) < __capacity) ? (index + __start) : ((index + __start) - __capacity)) + RAM_START_ADDRESS;
  __type out = 0;
  pReadBytes(&hsram1, pAdr, &out, 1);
  return out;
}

static FlagStatus IsFull()
{
	return (Size() == __capacity);
}

static FlagStatus IsEmpty()
{
	return (Size() == 0);
}

static uint32_t Size()
{
	return __size;
}

static uint32_t _increment(volatile uint32_t index)
{
  index++;
	if(index == __capacity){

		index = 0;
	}
	return index;
}

static uint32_t _decrement(volatile uint32_t index)
{
	(index == 0) ? (index = __capacity) : (index--);
	return index;
}

static FlagStatus _check_init()
{
//  if(__buffer == NULL)
//  {
//    printf("Circular buffer is not initialized by function \"Begin(capacity)\"!");
//    return RESET;
//  }
  return SET;
}

static void  pReadBytes(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pDstBuffer,
                                   uint32_t BufferSize)
{

  uint32_t size;
  __IO uint8_t *psramaddress = (uint8_t *)pAddress;
  uint8_t *pdestbuff = pDstBuffer;

  for (size = BufferSize; size != 0U; size--)
  {
    *pdestbuff = *psramaddress;
    pdestbuff++;
    psramaddress++;
  }

}

static void pWriteBytes(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pSrcBuffer,
                                    uint32_t BufferSize)
{

  uint32_t size;
  __IO uint8_t *psramaddress = (uint8_t *)pAddress;

  uint8_t *psrcbuff = pSrcBuffer;
  //__HAL_LOCK(hsram);
  for (size = BufferSize; size != 0U; size--)
  {
    *psramaddress = *psrcbuff;
    psrcbuff++;
    psramaddress++;
  }
  //__HAL_UNLOCK(hsram);
}

ExternRAM_Status ExternRAM_Test()
{
  uint8_t i, res = EXTERN_RAM_OK;
  uint32_t *adr32;
  //HAL_Delay(250);
  /* адресный тест */
  /* запись */
  adr32 = (uint32_t *)RAM_START_ADDRESS;
  i = 0;
  while ((uint32_t)adr32 <= RAM_END_ADDRESS)
  {
    *adr32 = i + (i << 16);
    i++;
    adr32++;
  }
  //HAL_Delay(250);
//  adr32 = (uint32_t *)START_ADR_SD_BUF;
//  i = 0;
//  while ((uint32_t)adr32 <= END_ADR_SD_BUF)
//  {
//    *adr32 = i + (i << 16);
//    i++;
//    adr32++;
//  }
  /* проверка */
  adr32 = (uint32_t *)RAM_START_ADDRESS;
  i = 0;
  while ((uint32_t)adr32 <= RAM_END_ADDRESS)
  {
    if ((i + (i << 16)) != *adr32)
    {
      res |= ERROR_ETH_BUF_RAM;
      break;
    }
    i++;
    adr32++;
  }
//  adr32 = (uint32_t *) START_ADR_SD_BUF;
//  i = 0;
//  while ((uint32_t) adr32 <= END_ADR_SD_BUF)
//  {
//    if ((i + (i << 16)) != *adr32)
//    {
//      res |= ERROR_SD_BUF_RAM;
//      break;
//    }
//    i++;
//    adr32++;
//  }
  ExternRAM_Erase();          // стирание памяти на всякий случай

  return res;
}

void ExternRAM_Erase()
{
  uint32_t *adr32 = (uint32_t *)RAM_START_ADDRESS;

  while ((uint32_t)adr32 <= RAM_END_ADDRESS)
  {
    *adr32 = 0;
    adr32++;
  }

}

