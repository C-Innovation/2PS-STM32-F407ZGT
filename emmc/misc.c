/*
 ******************************************************************************
 * @file    misc.c
 * @author  sizov.i
 * @version V1.0.0
 * @date    19.05.2017
 * @brief   This file contains all the MISC firmware functions.
 ******************************************************************************
 ******************************************************************************
 * <br><br>
 *
 *
 * <h2><center>&copy; COPYRIGHT 2017 Filtr KTV </center></h2>
 ******************************************************************************
 * FILE misc.c
 */

/* ------------------------     Includes     ---------------------------------------------- */

#include "main.h"

/* ------------------------     Private_Functions     ------------------------------------- */

/*
 * @brief  ковертирует в ASCII
 * @param  dataout - массив куда будут складываться преобразования
 * @param  datain - входное число
 * @param  size - количество символов
 * @return количество данных
 * @note   если количество получаемых цифр равно нулю, преобразуются все доступные
 *         преобразование без указания размера, занимает больше времени
 */
uint8_t convert_to_ascii(uint8_t* dataout, uint64_t datain, uint8_t size)
{
  uint8_t i, res, tmp8;

  if (size)          // нужно преобразовать определенное количество
  {
    res = size;
    while (size)
    {
      dataout[size - 1] = (uint8_t) ((datain % 10) + '0');
      datain /= 10;
      size--;
    }
  }
  else
  {
    i = 0;
    while (datain != 0)          // конвертируем все имеющиеся данные
    {
      dataout[i] = (uint8_t) ((datain % 10) + '0');
      datain /= 10;
      i++;
    }
    res = i;
    i = 0;
    while (i < (res / 2))          // данные получились в обратном порядке, исправим
    {
      tmp8 = dataout[i];
      dataout[i] = dataout[res - i - 1];
      dataout[res - i - 1] = tmp8;
      i++;
    }
    if (res == 0)          // если число нулевое
    {
      dataout[0] = '0';
      res = 1;
    }
  }
  dataout[res++] = 0; // допишем \0 чтобы читалось как строка

  return res;
}

/*
 * @brief  конвертирует в HEX
 * @param  dataout - массив куда будут складываться преобразования
 * @param  datain - входное число
 * @param  size - количество символов
 * @return количество данных
 * @note   если количество получаемых цифр равно нулю, преобразуются все доступные
 *         преобразование без указания размера, занимает больше времени
 */
uint8_t convert_to_hex(uint8_t* dataout, uint64_t datain, uint8_t size)
{
  uint8_t i, res, tmp8;

  if (size)          // нужно преобразовать определенное количество
  {
    res = size;
    while (size > 0)
    {
      if ((datain & 0xF) > 0x9)
      {
        dataout[size - 1] = (uint8_t) ((datain & 0xF) + 55);
      }
      else
      {
        dataout[size - 1] = (uint8_t) ((datain & 0xF) + '0');
      }
      datain = datain >> 4;
      size--;
    }
  }
  else
  {
    i = 0;
    while (datain != 0)          // конвертируем все имеющиеся данные
    {
      if ((datain & 0xF) > 0x9)
      {
        dataout[i] = (uint8_t) ((datain & 0xF) + 55);
      }
      else
      {
        dataout[i] = (uint8_t) ((datain & 0xF) + '0');
      }
      datain = datain >> 4;
      i++;
    }
    res = i;
    i = 0;
    while (i < (res / 2))          // данные получились в обратном порядке, исправим
    {
      tmp8 = dataout[i];
      dataout[i] = dataout[res - i - 1];
      dataout[res - i - 1] = tmp8;
      i++;
    }
    if (res == 0)          // если число нулевое
    {
      dataout[0] = '0';
      res = 1;
    }
  }

  return res;
}

/*
 * @brief  преобразует ASCII в число
 * @param  datain - массив с кодами
 * @param  count - количество символов, если нуль вернет количество
 * @return число
 */
uint32_t convert_from_ascii(uint8_t* datain, uint8_t* count)
{
  uint8_t tmp8, size;
  uint16_t res = 0;

  size = *count;
  if (size)
  {
    while (size > 0)
    {
      res *= 10;
      tmp8 = *datain - '0';
      if (tmp8 <= 9)
      {
        res += tmp8;
      }
      datain++;
      size--;
    }
  }
  else
  {
    while (*datain >= '0')
    {
      size++;
      res *= 10;
      tmp8 = *datain - '0';
      if (tmp8 <= 9)
      {
        res += tmp8;
      }
      else
      {
        res /= 10;
        size--;
        break;
      }
      datain++;
    }
    *count = size;
  }

  return res;
}

/*
 * @brief  преобразует HEX в число
 * @param  datain - массив с кодами
 * @param  count - количество символов, если нуль вернет количество
 * @return число
 */
uint32_t convert_from_hex(uint8_t* datain, uint8_t* count)
{
  uint8_t size;
  uint32_t res = 0;

  size = *count;
  if (size)
  {
    while (size > 0)
    {
      res = res << 4;
      if (*datain <= 0x39)
      {
        res += (*datain - '0') & 0xf;
      }
      else if ((*datain >= 'A') && (*datain <= 'F'))
      {
        res += (*datain - 55) & 0xf;
      }
      else if ((*datain >= 'a') && (*datain <= 'f'))
      {
        res += (*datain - 87) & 0xf;
      }
      datain++;
      size--;
    }
  }
  else
  {
    while (*datain >= '0')
    {
      size++;
      res = res << 4;
      if (*datain <= '9')
      {
        res += (*datain - '0') & 0xf;
      }
      else if ((*datain >= 'A') && (*datain <= 'F'))
      {
        res += (*datain - 55) & 0xf;
      }
      else if ((*datain >= 'a') && (*datain <= 'f'))
      {
        res += (*datain - 87) & 0xf;
      }
      else
      {
        res = res >> 4;
        size--;
        break;
      }
      datain++;
    }
    *count = size;
  }

  return res;
}

/* END OF FILE misc.c */
