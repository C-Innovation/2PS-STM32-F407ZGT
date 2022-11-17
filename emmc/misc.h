/*
 ******************************************************************************
 * @file    misc.h
 * @author  sizov.i
 * @version V1.0.0
 * @date    19.05.2017
 * @brief   This file contains all the functions prototypes for the
 *          MISC firmware library.
 ******************************************************************************
 ******************************************************************************
 * <br><br>
 *
 *
 * <h2><center>&copy; COPYRIGHT 2017 Filtr KTV </center></h2>
 ******************************************************************************
 * FILE misc.h
 */

/* ------------------------     Define to prevent recursive inclusion     ----------------- */

#ifndef INCLUDE_MISC_H_
#define INCLUDE_MISC_H_

/* ------------------------     Exported_Functions     ------------------------------------ */

uint8_t convert_to_ascii (uint8_t* dataout, uint64_t datain, uint8_t size);
uint8_t convert_to_hex (uint8_t* dataout, uint64_t datain, uint8_t size);
uint32_t convert_from_ascii(uint8_t* datain, uint8_t* count);
uint32_t convert_from_hex(uint8_t* datain, uint8_t* count);

#endif /* INCLUDE_MISC_H_ */

/* END OF FILE misc.h */
