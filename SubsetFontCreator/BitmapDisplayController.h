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
*	Created by Jon Mackey on 7/6/19.
*	Copyright © 2019 Jon. All rights reserved.
*/


#ifndef BitmapDisplayController_h
#define BitmapDisplayController_h

#include <stdio.h>
#include "DisplayController.h"

class BitmapDisplayController : public DisplayController
{
public:
							BitmapDisplayController(
								uint16_t				inRows,
								uint16_t				inColumns,
								uint16_t				inBytesPerRow,
								uint8_t*				inData);
	virtual void			MoveTo(
								uint16_t				inRow,
								uint16_t				inColumn);
	virtual void			MoveToRow(
								uint16_t				inRow);
	virtual void			MoveToColumn(
								uint16_t				inColumn);
	/*
	*	Turns the display off.
	*/
	virtual void			Sleep(void){};
	/*
	*	Turns the display on.
	*/
	virtual void			WakeUp(void){};
	/*
	*	ClearPixels: Sets a run of inPixelsToFill to inFillColor from the
	*	current position and column clipping.
	*/
	virtual void			FillPixels(
								uint16_t				inPixelsToFill,
								uint16_t				inFillColor);
	/*
	*	SetColumnRange: Sets a the absolute column range clipping to
	*	inStartColumn to inEndColumn.
	*/
	virtual void			SetColumnRange(
								uint16_t				inStartColumn,
								uint16_t				inEndColumn);
	/*
	*	SetRowRange: Sets a the absolute row range clipping to
	*	inStartRow to inEndRow.
	*/
	virtual void			SetRowRange(
								uint16_t				inStartRow,
								uint16_t				inEndRow);
	/*
	*	StreamCopy: Blindly copies inPixelsToCopy data bytes from inDataStream
	*	starting at the current row and column.  No checking to see if the data
	*	will fit on the display without clipping, skewing or wrapping.  The
	*	current row and column will be undefined (not updated within the class.)
	*/
	virtual void			StreamCopy(
								DataStream*				inDataStream,
								uint16_t				inPixelsToCopy);
	/*
	*	Simulate either vertical or horizontal display addressing
	*/
	virtual void			SetAddressingMode(
								EAddressingMode			inAddressingMode);
	void					Set1BitRotatedHorizontal(
								bool					in1BitRotatedHorizontal)
								{m1BitRotatedHorizontal = in1BitRotatedHorizontal;}
protected:
	bool		m1BitRotatedHorizontal;	// 8 pixel high strips (simulates 1 bit horizontal addressing)
	uint32_t	mBytesPerRow;	// of bitmap
	uint32_t*	mCurrent;
	uint32_t*	mCurrentRow;	// Only used in Horizontal address mode
	uint32_t*	mStartRow;
	uint32_t*	mEndRow;
	uint32_t*	mEndOfData;
	uint32_t	mStartColumn;
	uint32_t	mCurrentColumn;	// Only used in Vertical address mode
	uint32_t	mEndColumn;
	uint8_t*	mData;
	
	void					WritePixels(
								const uint16_t*			inPixels,
								uint16_t				inPixelsToWrite);

};
#endif /* BitmapDisplayController_h */
