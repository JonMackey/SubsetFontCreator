/*
*	AT24C.h, Copyright Jonathan Mackey 2019
*	Class to access data on the AT24C family of chips.
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

#ifndef AT24C_H
#define AT24C_H

// AT24C01A -> C16A aren't supported
// Only tested with C32, C128, and C256 (32, 64 and 64 byte pages resp.)
#define DEBUG_AT24C 1
class AT24C
{
public:
							AT24C(
								uint8_t					inDeviceAddress,
								uint8_t					inCapacity); // KB, one of: 4, 8, 16, 32, 64, 128
	uint16_t				Read(
								uint16_t				inDataAddress,
								uint16_t				inLength,
								uint8_t*				outBuffer);
	uint16_t				Write(
								uint16_t				inDataAddress,
								uint16_t				inLength,
								const uint8_t*			inBuffer);
	/*
	*	Rather than use some large software delay, this routine polls the AT24C
	*	chip waiting for it to return 0 after it enables itself after writing.
	*/
	bool					WaitTillReady(void);
#ifdef DEBUG_AT24C
	uint32_t				MaxWaitTime(void)
								{return(mMaxWaitTime);}
	uint32_t mMaxWaitTime;
#endif
private:
	uint8_t		mDeviceAddress;	// 0x50 + N low address bits.
								// 3 bits for C32 -> C64, 2 bits for C128 -> C512
	uint16_t	mPageSize;		// Initialized to one of: 32, 64, 128
};

#endif