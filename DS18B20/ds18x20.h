#include "../types.h"

#ifndef DS18X20_H_
#define DS18X20_H_


/* return values */
#define DS18X20_OK                0x00
#define DS18X20_ERROR             0x01

#define DS18X20_SKIP_ROM		  0xCC

#define DS18X20_CONVERT_T         0x44
#define DS18X20_READ              0xBE
#define DS18X20_WRITE			  0x4E


uint8_t DS18X20_int();
uint8_t DS18X20_get_temp( TTEMP *temp );
void DSTempToStr(TTEMP *temp, uint8_t DStempLSB, uint8_t DStempMSB);

#endif
