//
/*******************************************************************************
	License
	****************************************************************************
	This program is free software; you can redistribute it
	and/or modify it under the terms of the GNU General
	Public License as published by the Free Software
	Foundation; either version 3 of the License, or
	(at your option) any later version.
 
	This program is distributed in the hope that it will
	be useful, but WITHOUT ANY WARRANTY; without even the
	implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public
	License for more details.
 
	Licence can be viewed at
	http://www.gnu.org/licenses/gpl-3.0.txt

	Please maintain this license information along with authorship
	and copyright notices in any redistribution of this code
*******************************************************************************/
/*
*	BitmapDisplayController
*
*	This class was written to debug the Arduino classes.
*	
*	Created by Jon Mackey on 7/6/19.
*	Copyright © 2019 Jon. All rights reserved.
*/
#include "BitmapDisplayController.h"
#include "DataStream.h"
#include <string.h>

/*
*	The bitmap used is 32 bits per pixel.  The arduino classes are 16 bit 565
*	so everything needs to be expanded to 32 bits.
*/

/************************** BitmapDisplayController ***************************/
BitmapDisplayController::BitmapDisplayController(
	uint16_t	inRows,
	uint16_t	inColumns,
	uint16_t	inBytesPerRow,
	uint8_t*	inData)
	: DisplayController(inRows, inColumns),
	  mBytesPerRow(inBytesPerRow), mData(inData), m1BitRotatedHorizontal(false)
{
	mCurrentRow = mStartRow = mCurrent = (uint32_t*)mData;
	mStartColumn = 0;
	mEndOfData = mEndRow = (uint32_t*)&mData[mBytesPerRow * inRows];
	mEndColumn = inColumns;
}

/***************************** SetAddressingMode ******************************/
/*
*	The default mode is horizontal.  In horizontal mode only the mStartRow
*	changes leaving the mEndRow as a pointer to the end of the bitmap buffer.
*	The mCurrent pointer increases by 1 for each byte written.  When the end
*	column is reached mStartRow is incremented and mCurrent is set to mStartRow.
*
*	In vertical mode the mStartRow and mEndRow changes.  The mCurrent pointer
*	increases by mBytesPerRow for each byte written.  When mEndRow is reached
*	mCurrent is set to mStartRow + mCurrent + mEndRow - mStartRow
*/
void BitmapDisplayController::SetAddressingMode(
	EAddressingMode	inAddressingMode)
{
	if (inAddressingMode != mAddressingMode)
	{
		mAddressingMode = inAddressingMode;
		if (inAddressingMode == eHorizontal)
		{
			mEndRow = mEndOfData;
		} else
		{
			mEndColumn = mColumns;
			mCurrentColumn = mStartColumn;
		}
	}
}

/******************************* Pixel16To24 ***********************************/
uint32_t Pixel16To24(
	uint16_t	inPixel16)
{
	return(0xFF000000 + ((inPixel16 & 0xF800) << 8) + ((inPixel16 & 0x7E0) << 5) + ((inPixel16 & 0x1F) << 3));
}

/********************************* FillPixels *********************************/
void BitmapDisplayController::FillPixels(
	uint16_t	inPixelsToFill,
	uint16_t	inFillColor)
{
	uint32_t*	current = mCurrent;
	uint32_t	fillColor = Pixel16To24(inFillColor);
	while (inPixelsToFill)
	{
		uint32_t	rowPixels = (uint32_t)(&mCurrentRow[mEndColumn] - current);
		bool	atEndOfRow = inPixelsToFill >= rowPixels;
		if (atEndOfRow)
		{
			inPixelsToFill -= rowPixels;
		} else
		{
			rowPixels = inPixelsToFill;
			inPixelsToFill = 0;
		}
		uint32_t*	endOfRow = &current[rowPixels];
		while (current != endOfRow)
		{
			*(current++) = fillColor;
		}
		if (atEndOfRow)
		{
			// Advance to the next row
			mCurrentRow = (uint32_t*)&((uint8_t*)mCurrentRow)[mBytesPerRow];
			// Wrap if at last row
			if (mCurrentRow == mEndRow)
			{
				mCurrentRow = mStartRow;
			}
			// Point to the start of the new row
			current = &mCurrentRow[mStartColumn];
		}
	}
	mCurrent = current;
}

/*********************************** MoveTo ***********************************/
// No bounds checking.  Blind move.
void BitmapDisplayController::MoveTo(
	uint16_t	inRow,
	uint16_t	inColumn)
{
	MoveToRow(inRow);
	mColumn = inColumn;
}

/********************************* MoveToRow **********************************/
// No bounds checking.  Blind move.
void BitmapDisplayController::MoveToRow(
	uint16_t inRow)
{
	mStartRow = (uint32_t*)&mData[mBytesPerRow * inRow];
	mRow = inRow;
}

/******************************** MoveToColumn ********************************/
// No bounds checking.  Blind move.
// Doesn't actually make any changes to the controller.  Used by  other
// functions to set the start column relative to mColumn.
// (e.g. the relative version of SetColumnRange as defined in DisplayController)
void BitmapDisplayController::MoveToColumn(
	uint16_t inColumn)
{
	mColumn = inColumn;
}

/******************************* SetColumnRange *******************************/
// No bounds checking (in Arduino classes.)  Blind move.
void BitmapDisplayController::SetColumnRange(
	uint16_t	inStartColumn,
	uint16_t	inEndColumn)
{
	if (inStartColumn >= mColumns)
	{
		fprintf(stderr, "Error: SetColumnRange start column out of range.\n");
		return;
	} else if (inEndColumn < inStartColumn)
	{
		fprintf(stderr, "Error: SetColumnRange end column less than start column.\n");
		return;
	}
	mCurrentRow = mStartRow;
	mStartColumn = inStartColumn;
	mEndColumn = inEndColumn+1;
	mCurrent = &mCurrentRow[mStartColumn];
}

/******************************** SetRowRange *********************************/
// No bounds checking (in Arduino classes.)  Blind move.
void BitmapDisplayController::SetRowRange(
	uint16_t	inStartRow,
	uint16_t	inEndRow)
{
	if (inStartRow >= mRows)
	{
		fprintf(stderr, "Error: SetRowRange start row out of range.\n");
		return;
	} else if (inEndRow < inStartRow)
	{
		fprintf(stderr, "Error: SetRowRange end row less than start row.\n");
		return;
	}
	mCurrentRow = mStartRow = (uint32_t*)&mData[mBytesPerRow * inStartRow];
	mEndRow = (uint32_t*)&mData[mBytesPerRow * (inEndRow+1)];
	mEndColumn = mColumns;
	mCurrent = &mCurrentRow[mColumn];
	mCurrentColumn = mColumn;
}

/******************************** WritePixels *********************************/
/*
*	Simulation of the small buffer writes that take place on the Arduino
*	display controller classes.
*/
void BitmapDisplayController::WritePixels(
	const uint16_t*	inPixels,
	uint16_t		inPixelsToWrite)
{
	uint32_t*	current = mCurrent;
	if (mAddressingMode == eHorizontal)
	{
		if (m1BitRotatedHorizontal)
		{
			/*
			*	1 bit rotated horizontal simulates a 1 bit display in horizontal
			*	addressing mode.  In this mode these controllers display 8
			*	pixels vertically in rows (8 pixel rows per display page.)
			*	inPixelsToWrite will always be a multiple of 8.
			*	Each pixel written will increment to the next bitmap row till 8
			*	rows are written, then the column is incremented moving back to
			*	the start row.  This continues till the end of the row
			*	(mEndColumn).  When the end of the row is reached, the column is
			*	reset to mStartColumn
			*/
			uint32_t*	currentColumn = current;
			while (inPixelsToWrite)
			{
				for (int bits = 8; bits; bits--)
				{
					*current = Pixel16To24(*(inPixels++));
					current = (uint32_t*)&((uint8_t*)current)[mBytesPerRow];
				}
				inPixelsToWrite -= 8;
				currentColumn++;
				/*
				*	If we're at the end of the row THEN
				*	increment to the next page (set of 8 rows.)
				*/
				if (currentColumn == &mCurrentRow[mEndColumn])
				{
					mCurrentRow = (uint32_t*)&((uint8_t*)mCurrentRow)[mBytesPerRow*8];
					currentColumn = &mCurrentRow[mStartColumn];
				}
				current = currentColumn;
			}
		} else
		{
			while (inPixelsToWrite)
			{
				uint32_t	rowPixels = (uint32_t)(&mCurrentRow[mEndColumn] - current);
				bool	atEndOfRow = inPixelsToWrite >= rowPixels;
				if (atEndOfRow)
				{
					inPixelsToWrite -= rowPixels;
				} else
				{
					rowPixels = inPixelsToWrite;
					inPixelsToWrite = 0;
				}
				uint32_t*	endOfRow = &current[rowPixels];
				while (current != endOfRow)
				{
					*(current++) = Pixel16To24(*(inPixels++));
				}
				if (atEndOfRow)
				{
					// Advance to the next row
					mCurrentRow = (uint32_t*)&((uint8_t*)mCurrentRow)[mBytesPerRow];
					// Wrap if at last row
					if (mCurrentRow == mEndRow)
					{
						mCurrentRow = mStartRow;
					}
					// Point to the start of the new row
					current = &mCurrentRow[mStartColumn];
				}
			}
		}
	} else
	{
		while (inPixelsToWrite)
		{
			uint32_t	columnPixels = (uint32_t)((uint8_t*)mEndRow - (uint8_t*)current + mBytesPerRow - 1)/mBytesPerRow;
			bool	atEndOfColumn = inPixelsToWrite >= columnPixels;
			if (atEndOfColumn)
			{
				inPixelsToWrite -= columnPixels;
			} else
			{
				columnPixels = inPixelsToWrite;
				inPixelsToWrite = 0;
			}
			uint32_t*	endOfColumn = (uint32_t*)&((uint8_t*)current)[mBytesPerRow*columnPixels];
			while (current < endOfColumn)
			{
				*current = Pixel16To24(*(inPixels++));
				current = (uint32_t*)&((uint8_t*)current)[mBytesPerRow];
			}
			if (atEndOfColumn)
			{
				// Advance to the next column, top row
				mCurrentColumn++;
				// Wrap if at last column
				if (mCurrentColumn == mEndColumn)
				{
					mCurrentColumn = mStartColumn;
				}
				current = &mStartRow[mCurrentColumn];
			}
		}
	}
	mCurrent = current;
}

/******************************** StreamCopy **********************************/
void BitmapDisplayController::StreamCopy(
	DataStream*	inDataStream,	// A 16 bit data stream
	uint16_t	inPixelsToCopy)
{
	/*
	*	A 32 word buffer is used to simulate the buffer used in the Arduino
	*	TFT_ST77XX display controller class.
	*/
	uint16_t	buffer[32];
	while (inPixelsToCopy)
	{
		uint16_t pixelsToWrite = inPixelsToCopy > 32 ? 32 : inPixelsToCopy;
		inPixelsToCopy -= pixelsToWrite;
		inDataStream->Read(pixelsToWrite, buffer);
		WritePixels(buffer, pixelsToWrite);
	}
}
