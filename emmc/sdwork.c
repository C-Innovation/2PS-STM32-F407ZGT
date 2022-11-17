/*
 ******************************************************************************
 * @file    sdwork.c
 * @author  kuzminov.v
 * @version V1.2.0
 * @date    02.04.2019
 * @brief   This file contains all the REPROGRAMM firmware functions.
 ******************************************************************************
 ******************************************************************************
 * <br><br>
 *
 *
 * <h2><center>&copy; COPYRIGHT 2019 Filtr KTV </center></h2>
 ******************************************************************************
 * FILE sdwork.c
 */

/* ------------------------     Includes     ---------------------------------------------- */
#include <string.h>



#include "sdwork.h"
//#include "user_diskio.h"
//#include "timers.h"
#include "misc.h"

#include "usart.h"

/* ------------------------     Private_Constant     -------------------------------------- */

static const TCHAR PathUpdate[]    = "0:/UPDATE";
static const TCHAR PathFileFlash[] = "0:/UPDATE/FLASH.BIN";
static const TCHAR PathFileCksum[] = "0:/UPDATE/CKSUM";
static const TCHAR PathFileInfo[]  = "0:/UPDATE/INFO";
static const TCHAR WorkDirName[]   = "Registered";

static const TCHAR mainch[COL_DISK_DRV][4] = { "0:/" };
//static const uint32_t adr_load_file = ADR_LOAD_FILE;

/* ------------------------     Private_Variables     ------------------------------------- */

static FATFS FATFS_Obj[COL_DISK_DRV];
static FIL fil_obj;
static FILINFO FileInfo;

ProdPasport_TypeDef ProdPasport __attribute__((section(".ramfunc"))) __attribute__ ((aligned (4)));

/* ------------------------     Private_Functions     ------------------------------------- */

/*
 * @brief  создает рабочую папку
 * @param  path - указатель на имя директории
 * @retval FRESULT
 */
static FRESULT SD_CreateDir(const TCHAR* path)
{
  FRESULT res;

  /* создать директорию в корне */
  res = f_chdir(&mainch[SD_DISK][0]);
  if (res == FR_OK)
  {
    res = f_mkdir(path);
    if ((res == FR_OK) || (res = FR_EXIST))
      res = f_chdir(path);          // переход в директорию
  }

  return res;
}

/*
 * @brief  создает рабочий файл
 * @param  none
 * @retval FRESULT
 */
static FRESULT SD_CreateFile(void)
{
  FRESULT res;
  DIR dir;
  char *f_name;
  static uint8_t new_f_name[_MAX_LFN + 1];
  uint8_t max_f_num = 0, f_num;
#if _USE_LFN
  static char lfn[_MAX_LFN + 1];
  *FileInfo.fname = *lfn;
  FileInfo.fsize = sizeof(lfn);
#endif

  res = f_getcwd(&lfn[0], _MAX_LFN + 1);
  if (f_opendir(&dir, lfn) == FR_OK)
  {
    while (1)
    {
      if ((f_readdir(&dir, &FileInfo) != FR_OK) || (FileInfo.fname[0] == 0))
        break;          // Останов цикла при ошибке или при достижении конца списка директории
      if (FileInfo.fname[0] == '.')
        continue;          // Игнорирование элемента '.'
#if _USE_LFN
      f_name = *FileInfo.fname ? FileInfo.fname : FileInfo.fname;
#else
      f_name = FileInfo.fname;
#endif
      f_num = convert_from_ascii(f_name, 0);
      if (f_num > max_f_num)
        max_f_num = f_num;
    }
    f_closedir(&dir);
    /* формируем имя новому файлу */
    if (!max_f_num)
      new_f_name[0] = '1';
    else
    {
      res = convert_to_ascii(f_name, (max_f_num + 1), 0);
      strncpy(new_f_name, f_name, res);
    }
    strcat(new_f_name, ".bdi");

    res = f_open(&fil_obj, new_f_name, FA_CREATE_ALWAYS | FA_WRITE);          // создать файл для записи
    if (res == FR_OK)
    {
      f_sync(&fil_obj);
      if (fil_obj.fptr != fil_obj.obj.objsize)          // переход в конец файла
      {
        f_lseek(&fil_obj, fil_obj.obj.objsize);
      }
    }
  }

  return res;
}

/*
 * @brief  проверяет контрольную сумму файла
 * @param  pathmain - полный путь файла который проверяем
 * @param  pathcksum - полный путь файла где лежит эталонная контрольная сумма
 * @retval FRESULT - вернет ноль, если суммы совпали
 */
/*
static FRESULT SD_ChecksumFile(const TCHAR* pathmain, const TCHAR* pathcksum)
{
  FRESULT res;
  uint32_t buffmas[512 / 4];
  uint32_t rescrc = 0, corectcrc = 0, i = 0;
  uint32_t loccount;
  FIL tmp_fil_obj;

  while (i < (sizeof(buffmas) / 4))
  {
    buffmas[i] = 0;
    i++;
  }
   считываем контрольную сумму
  res = f_open(&tmp_fil_obj, pathcksum, FA_READ);
  if (res == FR_OK)          // открыть новый
  {
    res = f_read(&tmp_fil_obj, buffmas, sizeof(buffmas), (UINT *) &loccount);
    if (res == FR_OK)
    {
      corectcrc = convert_from_hex((uint8_t *) &buffmas, (uint8_t *) &loccount);
    }
  }
  f_close(&tmp_fil_obj);
   считаем контрольную сумму
  if (res == FR_OK)
  {
    res = f_open(&tmp_fil_obj, pathmain, FA_READ);
    if (res == FR_OK)          // открыть новый
    {
      while (tmp_fil_obj.fptr < tmp_fil_obj.fsize)
      {
        res = f_read(&tmp_fil_obj, buffmas, sizeof(buffmas), (UINT *) &loccount);
        if (res == FR_OK)          // считываем данные размером с буфер
        {
          i = 0;
          while (i < ((loccount + 3) / 4))
          {
            rescrc += buffmas[i];
            buffmas[i] = 0;
            i++;
          }
        }
      }
    }
    f_close(&tmp_fil_obj);
  }
   проверка совпадения сумм при наличии обоих файлов
  if (res == FR_OK)
  {
    if (corectcrc != rescrc)
    {
      res = FR_INVALID_PARAMETER;
    }
  }

  return res;
}
*/

/*
 * @brief   загружает файл во внешнюю память
 * @param   pathmain: полный путь файла который проверяем
 * @param   pathcksum: полный путь файла где лежит эталонная контрольная сумма
 * @param   pathinfo: информация об изделии
 * @retval  FRESULT
 */
/*
static FRESULT SD_LoadFile(const TCHAR* pathmain, const TCHAR* pathcksum, const TCHAR* pathinfo)
{
  FRESULT res;
  uint8_t buffmas[512];
  uint32_t loccount, pos, i;
  FIL tmp_fil_obj;

  res = f_open(&tmp_fil_obj, pathcksum, FA_READ);
  if (res == FR_OK)          // открыть новый
  {
    res = f_read(&tmp_fil_obj, buffmas, sizeof(buffmas), (UINT *) &loccount);
    if (res == FR_OK)
    {
      ProdPasport.checksum = convert_from_hex(buffmas, (uint8_t *) &loccount);
    }
  }
  f_close(&tmp_fil_obj);

  if (res == FR_OK)
  {
    res = f_open(&tmp_fil_obj, pathmain, FA_READ);
    if (res == FR_OK)          // открыть новый
    {
      if (tmp_fil_obj.fsize < (EEPROM_SIZE * 1024))          // сравнение с размером EEPROM
      {
        res = f_read(&tmp_fil_obj, (void *) adr_load_file, EEPROM_SIZE * 1024, (UINT *) &loccount);
        if (res == FR_OK)          // считываем данные размером с буфер
        {
          ProdPasport.size = loccount;
          if (tmp_fil_obj.fptr != tmp_fil_obj.fsize)          // считали не все
          {
            res = 1;
          }
        }
      }
    }
    f_close(&tmp_fil_obj);
  }

  if (res == FR_OK)
  {
    res = f_open(&tmp_fil_obj, pathinfo, FA_READ);
    if (res == FR_OK)          // открыть новый
    {
      res = f_read(&tmp_fil_obj, buffmas, sizeof(buffmas), (UINT *) &loccount);
      if (res == FR_OK)
      {
        loccount = 0;
        for (i = 0; i < sizeof(ProdPasport.prod_name); i++)
          ProdPasport.prod_name[i] = 0;
        for (i = 0; i < sizeof(ProdPasport.prod_sn); i++)
          ProdPasport.prod_sn[i] = 0;
        for (i = 0; i < sizeof(ProdPasport.prog_date); i++)
          ProdPasport.prog_date[i] = 0;
        pos = strcspn(&buffmas[loccount], "\r\n");
        strncpy(ProdPasport.prod_name, &buffmas[loccount], pos);
        loccount += pos + 2;
        pos = strcspn(&buffmas[loccount], "\r\n");
        strncpy(ProdPasport.prod_sn, &buffmas[loccount], pos);
        loccount += pos + 2;
        pos = strcspn(&buffmas[loccount], "\r\n");
        strncpy(ProdPasport.prog_date, &buffmas[loccount], pos);
      }
    }
    f_close(&tmp_fil_obj);
  }

  return res;
}
*/

/*
 * @brief  подготавливает флешку к работе
 * @param  ndrv - номер диска
 * @retval FRESULT
 */
FRESULT SD_Train(uint8_t ndrv)
{
  FRESULT res;
  BYTE work[_MAX_SS];

  res = f_mount(&FATFS_Obj[ndrv], &mainch[ndrv][0], 1);
  if (res != FR_OK)
  {          // не очень хорошо, форматируем
    if (ndrv == SD_DISK)
    {
      if (!f_mkfs(&mainch[ndrv][0], FM_FAT32, 0, work, sizeof work))
        res = f_mount(&FATFS_Obj[ndrv], &mainch[ndrv][0], 1);
    }
    else
    {
      if (!f_mkfs(&mainch[ndrv][0], FM_SFD, 0, work, sizeof work))
        res = f_mount(&FATFS_Obj[ndrv], &mainch[ndrv][0], 1);
    }
  }

  return res;
}

/*
 * @brief  создает папку и вызывает функцию создания файла
 * @param  none
 * @retval FRESULT
 */
FRESULT SD_TrainFile(void)
{
  FRESULT res;

#if _VOLUMES > 1
  f_chdrive(SD_DISK);
#endif
  res = SD_CreateDir(&WorkDirName[0]);
  if (res == FR_OK)
  {
    res = SD_CreateFile();
  }

  return res;
}

/*
 * @brief  записать массив на SD-карту
 * @param  allwrite - 1: записать все что есть
 * @retval FRESULT
 */

FRESULT SD_WriteData(Ci_CircularBufferTypeDef* ci_buf, uint8_t allwrite)
{
  uint32_t partSize = 0;
  uint32_t bytesWr = 0;
  uint8_t* partData;
  FRESULT res = FR_INVALID_PARAMETER;

  if(!ci_buf)
    return res;

  while(!ci_buf->IsEmpty())
  {
    partSize = (ci_buf->Size() >= _MAX_SS) ? _MAX_SS : ci_buf->Size();
    partData = ci_buf->FrontArray(partSize);
    res = f_write(&fil_obj, partData, partSize, &bytesWr);
    ci_buf->PopFrontArray(partSize);

    free(partData);
  }

  if(allwrite)
    res = f_close(&fil_obj);
  // TODO: Temporary limit for debugging!
  else if(fil_obj.obj.objsize >= 32768)
  {
    res = f_close(&fil_obj);
    res = SD_CreateFile();
  }

  return res;
}
/*
 * @brief  записать массив на SD-карту
 * @param  allwrite - 1: записать все что есть
 * @retval FRESULT
 */
/*
FRESULT SD_WriteData(uint8_t allwrite)
{
  UINT tmp, tmp1;
  uint32_t sdstart, sdend, tmp32, newsize;
  FRESULT res = FR_INVALID_PARAMETER;

  ExternRAM_GetPos(&sdstart, &sdend);
  if (sdstart >= sdend)
  {
    tmp32 = sdstart - sdend;
  }
  else
  {
    tmp32 = (END_ADR_SD_BUF - sdend) + (sdstart - START_ADR_SD_BUF);
  }
  if ((tmp32 >= SD_BLOCK_WRITE ) || (allwrite))          // есть что писать или писать все
  {
    Led2_OffTime(SD_TIMEOUT_STEAM);
    if (tmp32 > SD_MAX_BLOCK_WRITE)          // ограничиваем максимальный блок
    {
      tmp32 = SD_MAX_BLOCK_WRITE;
    }
    newsize = fil_obj.fsize + tmp32;          // еще не 4Г
    if (newsize < fil_obj.fsize)
    {
      f_close(&fil_obj);
      SD_CreateFile();
    }
    if (tmp32 > (END_ADR_SD_BUF - sdend + 1))          // буфер то кольцевой
    {
      f_write(&fil_obj, (void *) sdend, (END_ADR_SD_BUF - sdend + 1), &tmp);
      res = f_write(&fil_obj, (void *) START_ADR_SD_BUF, (tmp32 - tmp), &tmp1);
      sdend = ((sdend + (tmp + tmp1)) & EXTERN_BUF_SIZE) + START_ADR_SD_BUF;
    }
    else
    {
      res = f_write(&fil_obj, (void *) sdend, tmp32, &tmp);
      sdend = ((sdend + tmp) & EXTERN_BUF_SIZE) + START_ADR_SD_BUF;
    }
  }
  ExternRAM_ChangePos(sdstart, sdend);          // освободить память

  return res;
}
*/

/*
 * @brief  запись изменений
 * @param  none
 * @retval FRESULT
 */
FRESULT SD_FinishWriteData(void)
{
  return f_sync(&fil_obj);
}

/*
 * @brief  получить количество свободных байт на диске
 * @param  ndrv - номер диска
 * @param  freesize - количество свободных байт на диске
 * @retval FRESULT
 */
FRESULT SD_GetFreeSpace(uint8_t ndrv, uint64_t* freesize)
{
  FRESULT res;
  FATFS *pFATFS_Obj;
  uint64_t tmp64 = 0;

  pFATFS_Obj = &FATFS_Obj[ndrv];
  res = f_getfree(&mainch[ndrv][0], (DWORD *) &tmp64, &pFATFS_Obj);
  if (res == FR_OK)
  {
    tmp64 = tmp64 * FATFS_Obj[ndrv].csize * 512;
    *freesize = tmp64;
  }

  return res;
}

/*
 * @brief  проверяет наличие файлов для перепрограммирования на SD-карте
 *         и вызывает функцию перепрограммирования при их наличии
 * @param  none
 * @retval none
 */
void SD_CheckProgFile(void)
{
  uint32_t tmpnv;

  if (!SD_ChecksumFile(PathFileFlash, PathFileCksum))
  {
    if (!SD_LoadFile(PathFileFlash, PathFileCksum, PathFileInfo))
    {
      if (ReadCheckSumRAM() == CalcCheckSumRAM())
      {
        f_unlink(PathFileFlash);
        f_unlink(PathFileCksum);
        f_unlink(PathFileInfo);
        f_unlink(PathUpdate);
        tmpnv = NVIC->ISER[0];
        NVIC->ICER[0] = tmpnv;                  // запрет прерываний
        NVIC->ICPR[0] = NVIC->ISPR[0];          // сброс ожидающих обработку
        //ReprogEEPROM();                         // обновление прошивки
      }
    }
  }
}

/*
 * @brief  Прочитать паспортные данные изделия
 * @param  drv: номер физического диска
 * @retval DRESULT
 */
DRESULT SD_ReadPasport(uint8_t drv)
{
  DRESULT res;
  uint32_t LogBlockNbr;

  res = disk_ioctl(drv, GET_SECTOR_COUNT, &LogBlockNbr); /* считывает количество блоков в SD-карте */
  if (res == RES_OK)
  {
    res = disk_read(drv, ProdPasport.eMMCbuff, LogBlockNbr, 1);
  }

  //ReadInfoEEPROM();

  return res;
}

/*
 * @brief  Сохранить паспортные данные изделия
 * @param  drv: номер физического диска
 * @retval None
 */
DRESULT SD_SavePasport(uint8_t drv)
{
  DRESULT res;
  uint32_t LogBlockNbr;
// TODO: Add timer counting
//  ProdPasport.work_time += globalsubtime;
  ProdPasport.turn_on_cnt++;
  res = disk_ioctl(drv, GET_SECTOR_COUNT, &LogBlockNbr); /* считывает количество блоков в SD-карте */
  if (res == RES_OK)
  {
    res = disk_write(drv, ProdPasport.eMMCbuff, LogBlockNbr, 1);
  }

  return res;
}

/*
 * @brief  создает рабочий файл
 * @param  none
 * @retval FRESULT
 */
FRESULT SD_CreatePasportFile(void)
{
  FRESULT res;
  FIL fil;
  uint8_t ProdRev[5] = PRODUCT_REV;
  uint8_t str[512] = { 0 };
  uint8_t str_time[16] = { 0 };
  struct
  {
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
    uint16_t mil_second;
  } s_time;

  ProdRev[3] = (UART_BAUD_RATE / 115200) + 0x30;

  res = f_open(&fil, "Product_Pasport.txt", FA_CREATE_ALWAYS | FA_WRITE);          // создать файл для записи
  if (res == FR_OK)
  {
    res = f_sync(&fil);
    if (res == FR_OK)
    {
      strcpy(str, "Product name:      ");
      strcat(str, ProdPasport.prod_name);
      strcat(str, "\r\n");
      strcat(str, "Serial number:     ");
      strcat(str, ProdPasport.prod_sn);
      strcat(str, "\r\n");
      strcat(str, "Firmware version:  v."); // xxx поменять, если изменится версия ПО
      strcat(str, ProdRev);
      strcat(str, "\r\n");
      strcat(str, "Programming date:  ");
      strcat(str, ProdPasport.prog_date);
      strcat(str, "\r\n");
      strcat(str, "Operating time:    ");
      s_time.mil_second = (ProdPasport.work_time % 1000);
      s_time.second     = (ProdPasport.work_time / 1000) % 60;
      s_time.minute     = (ProdPasport.work_time / 60000) % 60;
      s_time.hour       = (ProdPasport.work_time / 3600000);
      convert_to_ascii(str_time, s_time.hour, 0); // часов проработано
      strcat(str, str_time);
      strcat(str, " h. ");
      convert_to_ascii(str_time, s_time.minute, 0); // минут проработано
      strcat(str, str_time);
      strcat(str, " m. ");
      convert_to_ascii(str_time, s_time.second, 0); // секунд проработано
      strcat(str, str_time);
      strcat(str, " sec. ");
      convert_to_ascii(str_time, s_time.mil_second, 0); // милисекунд проработано
      strcat(str, str_time);
      strcat(str, " msec. \r\n");
      strcat(str, "Number of turn on: ");
      convert_to_ascii(str_time, ProdPasport.turn_on_cnt, 0); // милисекунд проработано
      strcat(str, str_time);
      strcat(str, "\0");
      f_write(&fil, str, strlen(str), 0);
      f_close(&fil);
    }
  }

  return res;
}

/* END OF FILE sdwork.c */
