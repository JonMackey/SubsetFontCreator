/*
*	AT24C.cpp, Copyright Jonathan Mackey 2019
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

#include "Arduino.h"
#include "AT24C.h"
#include <Wire.h>

/*********************************** AT24C ************************************/
AT24C::AT24C(
	uint8_t	inDeviceAddress,
	uint8_t	inCapacity)
	: mDeviceAddress(inDeviceAddress)
#ifdef DEBUG_AT24C
		, mMaxWaitTime(0)
#endif
{
	switch(inCapacity)
	{
		case 4:		// C32
		case 8:		// C64
			mPageSize = 32;
			break;
		case 16:	// C128
		case 32:	// C256
			mPageSize = 64;
			break;
		case 64:	// C512
			mPageSize = 128;
			break;
		case 128:	// C1024
			mPageSize = 256;
			break;
		default:
			mPageSize = 32;
			break;
	}
}

/************************************ Read ************************************/
uint16_t AT24C::Read(
	uint16_t	inDataAddress,
	uint16_t	inLength,
	uint8_t*	outBuffer)
{
	/*
	*	Setup the AT24C to inDataAddress
	*/
	Wire.beginTransmission(mDeviceAddress);
	Wire.write(inDataAddress >> 8);
	Wire.write(inDataAddress & 0xFF);
	Wire.endTransmission(true);
	
	uint16_t	bytesRead = 0;
	uint16_t	bytes2Read = inLength;
	while (bytes2Read)
	{
		// The most you can read/request from Wire is 32 bytes.
		bytesRead = Wire.requestFrom(mDeviceAddress, (uint8_t)(bytes2Read >= 32 ? 32 : bytes2Read), (uint8_t)true);
		if (bytesRead > 0)
		{
			bytes2Read -= bytesRead;
			for (uint8_t i = bytesRead; i != 0; i--)
			{
				*(outBuffer++) = (uint8_t)Wire.read();
			}
		} else
		{
			return(0);
		}
	}
	return(inLength);
}

/******************************* WaitTillReady ********************************/
/*
*	Waits till ready or a timeout occurs, whichever occurs first.
*	Returns true if ready, else false for a timeout.
*/
bool AT24C::WaitTillReady(void)
{
	uint32_t timeout = micros() + 10000;	// timeout after 10ms
	do
	{
		Wire.beginTransmission(mDeviceAddress);
		if (Wire.endTransmission(true))
		{
			continue;
		}
#ifdef DEBUG_AT24C
		uint32_t	waitTime = micros() - timeout + 10000;
		if (waitTime > mMaxWaitTime)
		{
			mMaxWaitTime = waitTime;
		}
#endif
		return(true);
	} while (timeout > micros());
#ifdef DEBUG_AT24C
	mMaxWaitTime = 10000;
#endif
	return(false);
}

/*********************************** Write ************************************/
uint16_t AT24C::Write(
	uint16_t		inDataAddress,
	uint16_t		inLength,
	const uint8_t*	inBuffer)
{
	/*
	*	Constraints:
	*	- Wire only allows you to write 32 bytes at a time (including the 2 byte data address).
	*	- The AT24Cxx only allows you to write to a single page at a time:
	*		C32/64 - 32 byte page (low 5 bits is the address within the page)
	*		C128/256 - 64 byte page (low 6 bits is the address within the page)
	*		C512 - 128 byte page (low 7 bits is the address within the page)
	*		C1024 - 256 byte page (low 8 bits is the address within the page)
	*/
	// mPageSize -1 results in one of 0x1F, 0x3F, 0x7F, 0xFF.  This value is
	// used as a mask to determine the bytes left in the current page.
	uint16_t	bytesLeftInPage = mPageSize - (inDataAddress & (mPageSize -1));
	uint16_t	bytesLeft2Write = inLength;
	uint16_t	bytes2Write;
	while (WaitTillReady() &&
		bytesLeft2Write)
	{
		bytes2Write = bytesLeftInPage > 30 ? 30 : bytesLeftInPage;
		Wire.beginTransmission(mDeviceAddress);
		Wire.write(inDataAddress >> 8);
		Wire.write(inDataAddress & 0xFF);
		if (bytes2Write > bytesLeft2Write)
		{
			bytes2Write = bytesLeft2Write;
		}
		Wire.write(inBuffer, bytes2Write);
		inBuffer += bytes2Write;
		inDataAddress += bytes2Write;
		bytesLeft2Write -= bytes2Write;
		bytesLeftInPage -= bytes2Write;
		if (bytesLeftInPage == 0)
		{
			bytesLeftInPage = mPageSize;
		}
		/*
		This was an attempt to continue to write to the eeprom till the page is
		full and only then end the transmission with a stop.  This doesn't work.
		
		while (bytesLeft2Write &&
			bytesLeftInPage)
		{
			Wire.endTransmission(false);
			Wire.beginTransmission(mDeviceAddress);
			bytes2Write = bytesLeftInPage > 32 ? 32 : bytesLeftInPage;
			if (bytes2Write > bytesLeft2Write)
			{
				bytes2Write = bytesLeft2Write;
			}
			Wire.write(inBuffer, bytes2Write);
			inBuffer += bytes2Write;
			inDataAddress += bytes2Write;
			bytesLeft2Write -= bytes2Write;
			bytesLeftInPage -= bytes2Write;
		}*/
		Wire.endTransmission(true);
	}
	return(bytesLeft2Write == 0 ? inLength : 0);
}
