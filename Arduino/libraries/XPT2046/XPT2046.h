/*
*	XPT2046.h, Copyright Jonathan Mackey 2022
*	XPT2046 Touch Screen Controller interface
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
#ifndef XPT2046_h
#define XPT2046_h
#include <SPI.h>
#include "PlatformDefs.h"
#include "MSPeriod.h"


class XPT2046
{
public:
							XPT2046(
								pin_t					inCSPin,
								pin_t					inPenIRQPin,
								uint16_t				inHeight,
								uint16_t				inWidth,
								uint16_t				inMinX,
								uint16_t				inMaxX,
								uint16_t				inMinY,
								uint16_t				inMaxY,
								bool					inInvertX = false,
								bool					inInvertY = false);

	void					begin(
								uint8_t					inRotation = 0);
	
	bool					DisplayTouched(void);
	bool					Read(
								uint16_t&				outX,
								uint16_t&				outY,
								uint16_t&				outZ);
	bool					Align(
								uint16_t				inDesiredOffset = 20);
	void					DumpMinMax(void) const;
	
protected:
	pin_t		mCSPin;
	pin_t		mPenIRQPin;
	uint8_t		mRotation;
	uint16_t	mRows;
	uint16_t	mColumns;
	/*
	*	One of the modules I have has X inverted, opposite of the display.
	*/
	bool		mInvertX;
	bool		mInvertY;
	enum
	{
		eXMin,
		eXMax,
		eYMin,
		eYMax
	};
	uint16_t	mMinMax[4];
	uint16_t	mAlignXY[4];	// Used by Align()
	MSPeriod	mDebouncePeriod;
	volatile port_t*	mChipSelPortReg;
	port_t		mChipSelBitMask;
	SPISettings	mSPISettings;
	static volatile bool	sDisplayTouched;


	bool					ReadRaw(
								uint16_t&				outX,
								uint16_t&				outY,
								uint16_t&				outZ);
	void					SetRotation(
								uint8_t					inRotation);
	inline void				BeginTransaction(void)
							{
								SPI.beginTransaction(mSPISettings);
								if (mCSPin >= 0)
								{
									*mChipSelPortReg &= ~mChipSelBitMask;
								}
								
							}

	inline void				EndTransaction(void)
							{
								if (mCSPin >= 0)
								{
									*mChipSelPortReg |= mChipSelBitMask;
								}
								SPI.endTransaction();
							}

	static void				DisplayTouchedISR(void);

};
#endif // XPT2046_h
