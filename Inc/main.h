/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct
{
  FlagStatus RD :1;                    //0 регистрация данных
  FlagStatus SDAvailable :1;           //1 карта установлена
  FlagStatus SDTrain :1;               //2 карта готова к использованию
  FlagStatus TS :1;                    //3 синхронизация времени
  FlagStatus FLE :1;                   //4 загрузка прошивки
  FlagStatus TM :1;                    //5 технологический режим
  FlagStatus LP :1;                    //6 потеря питания
  FlagStatus FCNE :1;                  //7 прошивка не прошла проверку контрольной суммы
  FlagStatus EthTO :1;                 //8 пора отправить данные из буфера
  FlagStatus EthBO :1;                 //9 в буфере много данных
  FlagStatus FW :1;                    //10 работает альтернативный стек, прошивка разгружена
  FlagStatus SF :1;                    //11 найден сервер
  FlagStatus ELA :1;                   //12 сетевой кабель подсоединен
  FlagStatus ERD :1;                   //13 был прием по сети
  FlagStatus RSS :1;                   //14 сброс системных служб
  FlagStatus FT :1;                    //15 файл подготовлен для записи
  FlagStatus RFS :1;                   //16 требуется повторить поиск сервера
  FlagStatus ESF1 :1;                  //17 формат передачи по сетевому интерфейсу
  FlagStatus HSE :1;                   //18 произведен запуск с внешним генератором
  FlagStatus LSE :1;                   //19 произведен запуск с внешним низкочастотным генератором
  FlagStatus HSEE :1;                  //20 произведен запуск кварца эзернета
  FlagStatus ERAMCW :1;                //21 внешнее ОЗУ исправно
  FlagStatus LE :1;                    //22 логирование включено
  FlagStatus ISD :1;                   //23 внутренная карта готова
  FlagStatus EFU :1;                   //24 ошибка в файле обновления
  FlagStatus FWE :1;                   //25 можно писать прошивку
  FlagStatus EROMCW :1;                //26 внешнее ПЗУ исправно
  FlagStatus USBAvailable :1;          //27 кабель USB подключен
  FlagStatus NU2 :4;
} FLDeviseStatus;

typedef union
{
  FLDeviseStatus FLag;
  uint32_t REG;
} DeviseStatus;

typedef enum
{
  HSE_OK = 0,
  HSI_OK,
  CPU_ERROR
} CPU_Status;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOG
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOG
#define LED3_Pin GPIO_PIN_5
#define LED3_GPIO_Port GPIOB
#define LED4_Pin GPIO_PIN_7
#define LED4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
