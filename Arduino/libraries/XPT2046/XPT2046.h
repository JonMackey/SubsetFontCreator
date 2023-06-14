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
	
	bool					PenStateChanged(void);
	inline bool				PenIsDown(void) const
								{return(!digitalRead(mPenIRQPin));}
	bool					Read(
								uint16_t&				outX,
								uint16_t&				outY,
								uint16_t&				outZ);
	bool					Align(
								uint16_t				inDesiredOffset = 20);
	void					StartAlign(void);
	bool					AlignmentReady(void) const;
	void					DumpMinMax(void) const;
	
	inline bool				InvertX(void) const
								{return(mInvertX);}
	void					ToggleInvertX(void);
	
	
	inline bool				InvertY(void) const
								{return(mInvertY);}
	void					ToggleInvertY(void);
	
	void					GetMinMax(
								uint16_t				outMinMax[4]) const;
	void					SetMinMax(
								const uint16_t			inMinMax[4]);
	
protected:
	pin_t		mCSPin;
	pin_t		mPenIRQPin;
	uint8_t		mRotation;
	uint16_t	mRows;
	uint16_t	mColumns;
	bool		mPenStateIsDown;
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
	static volatile bool	sPenStateChanged;


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

	static void				PenStateChangedISR(void);

};
#endif // XPT2046_h
