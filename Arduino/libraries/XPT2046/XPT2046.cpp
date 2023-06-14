/*
*	XPT2046.cpp, Copyright Jonathan Mackey 2022
*	XPT2046 Touch Screen Controller interface
*
*	Tested on ATMega, STM32 and ESP32.
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
#include <SPI.h>
#include "XPT2046.h"

volatile bool	XPT2046::sPenStateChanged;

/********************************** XPT2046 ***********************************/
XPT2046::XPT2046(
	pin_t		inCSPin,
	pin_t		inPenIRQPin,
	uint16_t	inHeight,
	uint16_t	inWidth,
	uint16_t	inMinX,
	uint16_t	inMaxX,
	uint16_t	inMinY,
	uint16_t	inMaxY,
	bool		inInvertX,
	bool		inInvertY)
	: mCSPin(inCSPin), mPenIRQPin(inPenIRQPin),
		mRows(inHeight), mColumns(inWidth),
		mSPISettings(2000000, MSBFIRST, SPI_MODE0),
		mMinMax{inMinX, inMaxX, inMinY, inMaxY},
		mInvertX(inInvertX), mInvertY(inInvertY)
{
}

/*********************************** begin ************************************/
void XPT2046::begin(
	uint8_t inRotation)
{
	SPI.begin();
	if (mCSPin >= 0)
	{
		mChipSelBitMask = digitalPinToBitMask(mCSPin);
		mChipSelPortReg = portOutputRegister(digitalPinToPort(mCSPin));
		digitalWrite(mCSPin, HIGH);
		pinMode(mCSPin, OUTPUT);
	}
	sPenStateChanged = false;
	SetRotation(inRotation);
	if (mPenIRQPin >= 0)
	{
		pinMode(mPenIRQPin, INPUT);
		attachInterrupt(digitalPinToInterrupt(mPenIRQPin), XPT2046::PenStateChangedISR, CHANGE);
	}
}

/******************************** SetRotation *********************************/
void XPT2046::SetRotation(
	uint8_t inRotation)
{
	inRotation &= 3;
	mRotation = inRotation;
	if (inRotation & 1)
	{
		uint16_t	temp = mRows;
		mRows = mColumns;
		mColumns = temp;
	}
}

/******************************** ToggleInvertX *******************************/
void XPT2046::ToggleInvertX(void)
{
	mInvertX = !mInvertX;
}

/******************************** ToggleInvertY *******************************/
void XPT2046::ToggleInvertY(void)
{
	mInvertY = !mInvertY;
}

/****************************** PenStateChanged *******************************/
bool XPT2046::PenStateChanged(void)
{
	bool penStateChanged = false;

	// Ensure that the change lasts more than mDebouncePeriod.
	if (sPenStateChanged)
	{
		if (!mDebouncePeriod.Get())
		{
			/*
			*	If the debounce period is too long you miss real changes to
			*	the pen state.  It's still not perfect.  Some of the state
			*	changes are missed on very light or fast touches.
			*/
			mDebouncePeriod.Set(1);	// 1ms
			mDebouncePeriod.Start();
			mPenStateIsDown = PenIsDown();
		} else if (mDebouncePeriod.Passed())
		{
			mDebouncePeriod.Set(0);
			penStateChanged = mPenStateIsDown == PenIsDown();
			sPenStateChanged = false;
		}
	}
	return(penStateChanged);
}

/********************************** ReadRaw ***********************************/
bool XPT2046::ReadRaw(
	uint16_t&	outX,
	uint16_t&	outY,
	uint16_t&	outZ)
{
	/*
	*	The documentation states that you can overlap commands and data.
	*	I found that the returned values are more consistent when commands are
	*	sent separately followed by the data received.	 When overlapped the
	*	data returned in many cases is garbage (bits set in the lower 3 bits.)
	*/
	enum
	{
		eZ1,
		eZ2,
		eX0,	// Ignored to allow for settling
		eY0,	// Ignored to allow for settling
		eX1,
		eY1,
		eNumCommands
	};
	struct SCmdData
	{
		uint8_t		cmd;
		uint16_t	data;
		void Adjust(void)
		{
			// The 12 bits of data starts on bit 6 as shown below.
			// x111 1111 1111 1xxx
		#if BYTE_ORDER == LITTLE_ENDIAN
			// (Big endian to little endian) >> 3
			data = ((data & 0xff00) >> 11) | ((data & 0xff) << 5);
		#else
			data >>= 3;
		#endif
		}
	} __attribute__ ((packed)) cmdData[eNumCommands] =
			{{0xB1},{0xC1},{0x91},{0xD1},{0x91},{0xD0}};
		//	Commands: (Summary as used. There are other options in doc.)
		//		Bx reads Z1
		//		Cx reads Z2
		//		9x reads X
		//		Dx reads Y
		//		A 1 in the low nibble keeps the chip active.
		//		A 0 in the low nibble puts the chip to sleep.

		// I noticed that some of the XPT2046 classes do averaging. Looking
		// at the data coming in, I didn't see any value in averaging.	The
		// data was just as error prone (but not bad.)
		// It would be easy to add averaging by simply inserting a few more
		// 0x91 0xD1 commands into the array above, and add corresponding
		// enum values.	 Then average the returned data.

	/*
	*	Within the SPI.transfer(), the PenIRQPin toggles between
	*	high and low several times during the SPI.transfer().
	*	Stop watching the mPenIRQPin
	*/
	detachInterrupt(digitalPinToInterrupt(mPenIRQPin));
	BeginTransaction();
	SPI.transfer(&cmdData[0].cmd, sizeof(cmdData));
	EndTransaction();
	/*
	*	Wait for the pen up by watching for the mPenIRQPin to go from low to
	*	high.
	*/
	attachInterrupt(digitalPinToInterrupt(mPenIRQPin), XPT2046::PenStateChangedISR, CHANGE);
	sPenStateChanged = false;

	for (uint8_t i = 0; i < eNumCommands; i++)
	{
		cmdData[i].Adjust();
	}
#if 0
	// Dump the raw values
	static char ll[] = "zzxyxyxyxyxyxyxyrrr";
	// i = 0 dump all, including z and the skipped xy values.
	// i = 4 dump just the used xy values.
	for (uint8_t i = 4; i < eNumCommands; i++)
	{
		Serial.print(ll[i]);
		Serial.print(cmdData[i].data);
		Serial.print(' ');
	}
	Serial.println();
#endif
	int32_t z = cmdData[eZ1].data + 0xFFF - cmdData[eZ2].data;

	/*
	*	The rotation applied below expects a certain orientation.  I decided on
	*	a default based on one of my displays.  Any display that is wired
	*	differently needs to be adjusted by setting mInvertX and/or mInvertY in
	*	the constructor.  It's easy to see when points need to be inverted.
	*
	*	I noticed no consistency with the display module manufacturers in terms
	*	of how the YN/YP and XN/XP pins are wired to the touch panel.  Swapping
	*	them obviously inverts the values received.
	*/
	if (mInvertX)
	{
		cmdData[eX1].data = 0xFFF - cmdData[eX1].data;
	}
	if (mInvertY)
	{
		cmdData[eY1].data = 0xFFF - cmdData[eY1].data;
	}


	switch (mRotation)
	{
		case 0: // 0
			outX = cmdData[eY1].data;
			outY = 0xFFF - cmdData[eX1].data;
			break;
		case 1: // 90
			outX = 0xFFF - cmdData[eX1].data;
			outY = 0xFFF - cmdData[eY1].data;
			break;
		case 2: // 180
			outX = 0xFFF - cmdData[eY1].data;
			outY = cmdData[eX1].data;
			break;
		case 3: // 270
			outX = cmdData[eX1].data;
			outY = cmdData[eY1].data;
			break;
	}
	outZ = z;
	bool	isValid = z >= 100;
	return(isValid);
}

/************************************ Read ************************************/
bool XPT2046::Read(
	uint16_t&	outX,
	uint16_t&	outY,
	uint16_t&	outZ)
{
	uint16_t	x,y,z;

	bool	isValid = ReadRaw(x,y,z);
	outZ = z;
	outX = map(x, mMinMax[eXMin], mMinMax[eXMax], 0, mColumns);
	outY = map(y, mMinMax[eYMin], mMinMax[eYMax], 0, mRows);

	return(isValid);
}

/***************************** PenStateChangedISR *****************************/
void XPT2046::PenStateChangedISR(void)
{
	sPenStateChanged = true;
}

/********************************* DumpMinMax *********************************/
void XPT2046::DumpMinMax(void) const
{
	for (uint16_t i = 0; i < 4; i++)
	{
		Serial.print(i & 2 ? 'Y':'X');
		Serial.print(i & 1 ? "Max = ":"Min = ");
		Serial.print(mMinMax[i]);
		Serial.println(';');
	}
}

/********************************* StartAlign *********************************/
void XPT2046::StartAlign(void)
{
	mAlignXY[0] = mAlignXY[1] = mAlignXY[2] = mAlignXY[3] = 0x7FF;
}

/******************************* AlignmentReady *******************************/
/*
*	Returns true when enough alignment points have been entered to save min&max.
*	True does NOT mean that the current min & max values are any good.  That can
*	only be determined by visually checking the points touched align with
*	the display, generally by drawing the point touched after each press.
*/
bool XPT2046::AlignmentReady(void) const
{
	return(mAlignXY[eXMin] != 0x7FF &&
			mAlignXY[eXMax] != 0x7FF &&
			mAlignXY[eYMin] != 0x7FF &&
			mAlignXY[eYMax] != 0x7FF);
}

/********************************** GetMinMax *********************************/
void XPT2046::GetMinMax(
	uint16_t	outMinMax[4]) const
{
	memcpy(outMinMax, mMinMax, sizeof(mMinMax));
}

/********************************** SetMinMax *********************************/
void XPT2046::SetMinMax(
	const uint16_t	inMinMax[4])
{
	memcpy(mMinMax, inMinMax, sizeof(mMinMax));
}

/*********************************** Align ************************************/
/*
*	This is used to align the touchscreen to the display.
*
*	To use:
*	Write code to display the x and y values returned by Read() to the display
*	or dump the values to Serial.  You just need something to display the points
*	as they're touched/read.
*
*	Call DisplayController::DrawAlignmentPoints(void) or an equivalent of your
*	own design, to draw an inset array of 9 points on the display as shown in
*	the example below.
*
*	Example, for rows = 320, columns = 480, and inset 20:
*
*					*	*	*	=	TL	TC	TR	=	20,20	240,20, 460,20
*					*	*	*	=	CL	CC	CR	=	20,160	240,160 460,160
*					*	*	*	=	BL	BC	BR	=	20,300	240,300 460,300
*
*	Fill the mAlignXY array by precisely touching CL, CR, TC, and BC points
*	drawn on the display, calling Align() after each touch.	 If you think you
*	didn't precisely touch any of the points, touch any point again (any order.)
*	Stop calling Align(), then touch these 4 points again, verifying that the
*	points returned by Read() are mapped correctly to your display.  If the
*	alignment is off, repeat the above for whatever axis is off. Once reasonably
*	accurate, call DumpMinMax() to get the updated values.	Pass these updated
*	values to the XPT2046 constructor.
*/
bool XPT2046::Align(
	uint16_t	inDesiredOffset)
{
	uint16_t	x,y,z;
	bool isValid = ReadRaw(x,y,z);
	if (isValid)
	{
		bool changeMade = true;
		switch (map(x, 0, 0xFFF, 0, 3))
		{
			case 0:		// Left 3rd of X axis touched
				mAlignXY[eXMin] = x;
				break;
			case 1:		// Center 3rd of X axis touched
				changeMade = false;
				break;
			default:	// Right 3rd of X axis touched
				mAlignXY[eXMax] = x;
				break;
		}
		if (changeMade &&
			mAlignXY[eXMin] != 0x7FF &&
			mAlignXY[eXMax] != 0x7FF)
		{
			mMinMax[eXMin] = map(0, inDesiredOffset, mColumns-inDesiredOffset,
									mAlignXY[eXMin], mAlignXY[eXMax]);
			mMinMax[eXMax] = map(mColumns, inDesiredOffset, mColumns-inDesiredOffset,
									mAlignXY[eXMin], mAlignXY[eXMax]);
		}
		changeMade = true;
		switch (map(y, 0, 0xFFF, 0, 3))
		{
			case 0:		// Top 3rd of Y axis touched
				mAlignXY[eYMin] = y;
				break;
			case 1:		// Center 3rd of Y axis touched
				changeMade = false;
				break;
			default:	// Bottom 3rd of Y axis touched
				mAlignXY[eYMax] = y;
				break;
		}
		if (changeMade &&
			mAlignXY[eYMin] != 0x7FF &&
			mAlignXY[eYMax] != 0x7FF)
		{
			mMinMax[eYMin] = map(0, inDesiredOffset, mRows-inDesiredOffset,
									mAlignXY[eYMin], mAlignXY[eYMax]);
			mMinMax[eYMax] = map(mRows, inDesiredOffset, mRows-inDesiredOffset,
									mAlignXY[eYMin], mAlignXY[eYMax]);
		}
	}
	return(isValid);
}

