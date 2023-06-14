/*
*	PlatformDefs.h, Copyright Jonathan Mackey 2022
*
*	GNU license:
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*	Please maintain this license information along with authorship and copyright
*	notices in any redistribution of this code.
*
*/
#ifndef PlatformDefs_h
#define PlatformDefs_h

#include <inttypes.h>
#include <string.h>
#ifndef __MACH__
	#include <Arduino.h>
	#if defined ESP_H
		#include <pgmspace.h>
		typedef uint32_t port_t;
		typedef int16_t pin_t;
	#elif  defined _STM32_DEF_
		#include <pgmspace.h>
		typedef uint32_t port_t;
		typedef uint32_t pin_t;
	#else
		#include <avr/pgmspace.h>
		typedef uint8_t port_t;
		typedef int8_t pin_t;
	#endif
#else
#ifndef memcpy_P
	#define memcpy_P memcpy
#endif
#include <math.h>
#endif
#endif // PlatformDefs_h
