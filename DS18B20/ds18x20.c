/*********************************************************************************
Title:    DS18X20-Functions via One-Wire-Bus
Author:   Martin Thomas <eversmith@heizung-thomas.de>   
          http://www.siwawi.arubi.uni-kl.de/avr-projects
Software: avr-gcc 4.3.3 / avr-libc 1.6.7 (WinAVR 3/2010) 
Hardware: any AVR - tested with ATmega16/ATmega32/ATmega324P and 3 DS18B20

Partly based on code from Peter Dannegger and others.

changelog:
20041124 - Extended measurements for DS18(S)20 contributed by Carsten Foss (CFO)
200502xx - function DS18X20_read_meas_single
20050310 - DS18x20 EEPROM functions (can be disabled to save flash-memory)
           (DS18X20_EEPROMSUPPORT in ds18x20.h)
20100625 - removed inner returns, added static function for read scratchpad
         . replaced full-celcius and fractbit method with decicelsius
           and maxres (degreeCelsius*10e-4) functions, renamed eeprom-functions,
           delay in recall_e2 replaced by timeout-handling
20100714 - ow_command_skip_last_recovery used for parasite-powerd devices so the
           strong pull-up can be enabled in time even with longer OW recovery times
20110209 - fix in DS18X20_format_from_maxres() by Marian Kulesza
**********************************************************************************/


#include "ds18x20.h"
#include "../OW/dallas_one_wire.h"
#include "../types.h"
#include <util/delay.h>
#include <stdlib.h>

void DSTempToStr(TTEMP *temp, uint8_t DStempLSB, uint8_t DStempMSB)
{

  unsigned short DStemp = (DStempMSB << 8 ) | DStempLSB;

  temp->cel = ( unsigned char )(( DStemp >> 4 ) & 0x7F );
  temp->fract = ( unsigned char )((( DStemp & 0xF ) * 625) / 1000 );

}

uint8_t DS18X20_int(){
	uint8_t isPresent = 0;

	isPresent = dallas_reset_delay();
		if(isPresent != 1)
			return DS18X20_ERROR;

	dallas_reset_delay();
	dallas_write_byte(DS18X20_SKIP_ROM);
	dallas_write_byte(DS18X20_WRITE);
	dallas_write_byte(0);
	dallas_write_byte(0);
	dallas_write_byte(0x1F);

	isPresent = dallas_reset_delay();
			if(isPresent != 1)
				return DS18X20_ERROR;

	return DS18X20_OK;
}

uint8_t DS18X20_get_temp( TTEMP *temp ){
	uint8_t isPresent = 0;

	uint8_t cel, fract;
	isPresent = dallas_reset_delay();
	if(isPresent != 1)
		return DS18X20_ERROR;

	dallas_skip_rom();
	dallas_write_byte(DS18X20_READ);
	cel = dallas_read_byte();
	fract = dallas_read_byte();

	dallas_reset_delay();
	dallas_write_byte(DS18X20_SKIP_ROM);
	dallas_write_byte(DS18X20_CONVERT_T);
	dallas_reset_delay();

	temp->cel = cel;
	temp->fract = fract;
	DSTempToStr(temp,cel,fract);
	return DS18X20_OK;
}
