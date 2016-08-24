/*
 * types.h

 *
 *  Created on: 5 maj 2015
 *      Author: Mateusz
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <avr/io.h>

typedef union {
	uint8_t bytes[7];
	struct {
		uint8_t ss;
		uint8_t mm;
		uint8_t hh;
		uint8_t dayofwek;
		uint8_t day;
		uint8_t month;
		uint8_t year;
	};
} TDATETIME;


typedef struct {
	int8_t cel;
	uint8_t fract;
} TTEMP;

#endif /* TYPES_H_ */
