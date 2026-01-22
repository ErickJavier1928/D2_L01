/*
 * _7SD.c
 *
 * Created: 1/22/2026 11:08:52 AM
 *  Author: Eliph
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "SSD.h"

void MAP_7SD(uint8_t NM){
	switch(NM)
	{
		case 0x00:
		PORTD = 0x3F; // 0
		break;
		case 0x01:
		PORTD = 0x06; // 1
		break;
		case 0x02:
		PORTD = 0x5B; // 2
		break;
		case 0x03:
		PORTD = 0x4F; // 3
		break;
		case 0x04:
		PORTD = 0x66; // 4
		break;
		case 0x05:
		PORTD = 0x6D; // 5
		break;
		case 0x06:
		PORTD = 0x7D; // 6
		break;
		case 0x07:
		PORTD = 0x07; // 7
		break;
		case 0x08:
		PORTD = 0x7F; // 8
		break;
		case 0x09:
		PORTD = 0x6F; // 9
		break;
		default:
		PORTD = 0x00; // blank for undefined
		break;
	}
}