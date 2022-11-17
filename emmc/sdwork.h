/*
 ******************************************************************************
 * @file    sdwork.h
 * @author  kuzminov.v
 * @version V1.2.0
 * @date    02.04.2019
 * @brief   This file contains all the functions prototypes for the
 *          SDWORK firmware library.
 ******************************************************************************
 ******************************************************************************
 * <br><br>
 *
 *
 * <h2><center>&copy; COPYRIGHT 2019 Filtr KTV </center></h2>
 ******************************************************************************
 * FILE sdwork.h
 */

/* ------------------------     Define to prevent recursive inclusion     ----------------- */

#ifndef SDWORK_H_
#define SDWORK_H_

/* ------------------------     Includes     ---------------------------------------------- */
//#include "main.h"
#include "ff.h"
#include "diskio.h"
#include "circular_buffer.h"
/* ------------------------     Exported_Constants     ------------------------------------ */

#define SD_TIMEOUT_STEAM        25
#define SD_BLOCK_WRITE          (uint32_t)8192
#define SD_MAX_BLOCK_WRITE      (uint32_t)30*8192
#define COL_DISK_DRV            _VOLUMES

/* ------------------------     Exported_Types     ---------------------------------------- */

typedef struct
{
  union
  {
    uint8_t EEPROMbuff[512];
    struct
    {
      uint32_t checksum;
      uint32_t size;
      uint8_t  prod_name[64];
      uint8_t  prod_sn[16];
      uint8_t  prog_date[12];
    };
  };
  union
  {
    uint8_t eMMCbuff[512];
    struct
    {
      uint64_t work_time;
      uint32_t turn_on_cnt;
    };
  };
} ProdPasport_TypeDef;

/* ------------------------     Exported_Functions     ------------------------------------ */

FRESULT SD_Train(uint8_t ndrv);
FRESULT SD_TrainFile(void);
FRESULT SD_WriteData(Ci_CircularBufferTypeDef* ci_buf, uint8_t allwrite);
FRESULT SD_FinishWriteData(void);
FRESULT SD_GetFreeSpace(uint8_t ndrv, uint64_t* freesize);
void SD_CheckProgFile(void);
DRESULT SD_ReadPasport(uint8_t drv);
DRESULT SD_SavePasport(uint8_t drv);
FRESULT SD_CreatePasportFile(void);

#endif /* SDWORK_H_ */

/* END OF FILE sdwork.h */
