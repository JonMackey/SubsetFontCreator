/*
*	DisplayController.cpp, Copyright Jonathan Mackey 2019-2023
*	Base class for a display controller.
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
#include "DisplayController.h"
#include "DataStream.h"
#ifndef __MACH__
#include <Arduino.h>
#else
#include <iostream>
#endif


/***************************** DisplayController ******************************/
DisplayController::DisplayController(
	uint16_t	inRows,
	uint16_t	inColumns)
	: mRows(inRows), mColumns(inColumns), mRow(0), mColumn(0),
	  mAddressingMode(eHorizontal), mFGColor(0xFFFF), mBGColor(0)
{
}

/********************************* CanMoveTo **********************************/
bool DisplayController::CanMoveTo(
	uint16_t	inRow,
	uint16_t	inColumn) const
{
	return(inRow < mRows && inColumn < mColumns);
}

/*********************************** MoveBy ***********************************/
/*
*	Returns true if the relative move was within the display bounds.
*/
bool DisplayController::MoveBy(
	uint16_t	inRows,
	uint16_t	inColumns)
{
	inRows += mRow;
	inColumns += mColumn;
	bool success = CanMoveTo(inRows, inColumns);
	if (success)
	{
		MoveTo(inRows, inColumns);
	}
	return(success);
}

/******************************** MoveColumnBy ********************************/
// Resets to zero on wrap.  Does not affect the row (page)
void DisplayController::MoveColumnBy(
	uint16_t	inMoveBy)
{
	uint16_t	newColumn = mColumn + inMoveBy;
	if (newColumn >= mColumns)
	{
		newColumn = 0;
	}
	MoveToColumn(newColumn);
}

/********************************* MoveRowBy **********************************/
// Resets to zero on wrap.  Does not affect the column
void DisplayController::MoveRowBy(
	uint16_t	inMoveBy)
{
	uint16_t	newRow = mRow + inMoveBy;
	if (newRow >= mRows)
	{
		newRow = 0;
	}
	MoveToRow(newRow);
}

/********************************** WillFit ***********************************/
bool DisplayController::WillFit(
	uint16_t	inRows,
	uint16_t	inColumns)
{
	return(	(mRow + inRows) <= mRows &&
			(mColumn + inColumns) <= mColumns);
}

/************************************ Clip ************************************/
/*
*	Clips/adjusts ioCoord and ioLen if either are out of the display bounds.
*	Returns the amount ioCoord was adjusted in the forward/positive direction.
*/
int32_t DisplayController::Clip(
	int32_t&	ioCoord,
	int32_t&	ioLen,
	int32_t		inMaxCoord)
{
	int32_t	coordDelta = 0;
	if (ioCoord < 0)
	{
		ioLen += ioCoord;
		coordDelta = -ioCoord;
		ioCoord = 0;
	}
	if (ioLen > 0 &&
		ioCoord < inMaxCoord)
	{
		int32_t	endCoord = inMaxCoord - ioCoord - ioLen;
		if (endCoord < 0)
		{
			ioLen += endCoord;
		}
	} else
	{
		ioLen = 0;
	}
	return(coordDelta);
}

/*********************************** ClipX ************************************/
/*
*	Clips/adjusts ioCoord and ioLen if either are out of the display bounds.
*	
*/
int32_t DisplayController::ClipX(
	int32_t&	ioXCoord,
	int32_t&	ioWidth)
{
	return(Clip(ioXCoord,ioWidth,mColumns));
}

/*********************************** ClipY ************************************/
/*
*	Clips/adjusts ioCoord and ioLen if either are out of the display bounds.
*	
*/
int32_t DisplayController::ClipY(
	int32_t&	ioYCoord,
	int32_t&	ioHeight)
{
	return(Clip(ioYCoord,ioHeight,mRows));
}

/************************************ Fill************************************/
void DisplayController::Fill(
	uint16_t	inFillColor)
{
	/*
	*	Note that the order of the calls matter.  MoveTo should be called before
	*	SetColumnRange because on the TFT displays SetColumnRange sends the
	*	command to setup writing mode.  If MoveTo were to be called after, 
	*	writing mode command is terminated before the fill starts.
	*	The order doesn't matter on the OLED display.
	*/
	MoveTo(0, 0);	// After clear the page and column will wrap to zero (no need to call MoveTo again at end.)
	SetColumnRange(0, mColumns-1);	// Reset the range in case it's been clipped.
	FillPixels((uint32_t)mRows*mColumns, inFillColor);
}

/********************************* FillBlock **********************************/
void DisplayController::FillBlock(
	uint16_t	inRows,
	uint16_t	inColumns,
	uint16_t	inFillColor)
{
	if ((inColumns+mColumn) >= mColumns)
	{
		inColumns = mColumns - mColumn;
	}
	if ((inRows+mRow) >= mRows)
	{
		inRows = mRows - mRow;
	}
	if (inColumns && inRows)
	{
		SetColumnRange(inColumns);
		// The column index will wrap back to the starting point.
		// The page won't so it needs to be reset.
		FillPixels((uint32_t)inRows * inColumns, inFillColor);
		//SetColumnRange(0, mColumns-1);	// Remove the column range clipping
		//MoveToRow(mRow);	// Leave the page unchanged
		MoveColumnBy(inColumns); // Advance by inColumns (or wrap to zero if at or past end)
	}
}

/********************************** FillRect **********************************/
void DisplayController::FillRect(
	uint16_t	inX,
	uint16_t	inY,
	uint16_t	inWidth,
	uint16_t	inHeight,
	uint16_t	inFillColor)
{
	MoveTo(inY, inX);
	FillBlock(inHeight, inWidth, inFillColor);
}

/******************************* FillTintedRect *******************************/
void DisplayController::FillTintedRect(
	uint16_t	inX,
	uint16_t	inY,
	uint16_t	inWidth,
	uint16_t	inHeight,
	uint8_t		inTint)
{
	MoveTo(inY, inX);
	FillBlock(inHeight, inWidth, Calc565Color(inTint));
}

/********************************* FillRect8 **********************************/
void DisplayController::FillRect8(
	const Rect8_t*	inRect,
	uint16_t		inFillColor)
{
	FillRect(inRect->x, inRect->y, inRect->width, inRect->height, inFillColor);
}

/****************************** DrawRoundedRect *******************************/
uint16_t DisplayController::DrawRoundedRect(
	int16_t	inX,
	int16_t	inY,
	int16_t	inWidth,
	int16_t	inHeight,
	int16_t	inRadius,
	uint8_t	inFillTint,
	bool	inFrameOnly)
{
	int16_t	radiusX2 = inRadius*2;
	int16_t widthMRX2 = inWidth-radiusX2;
	int16_t heightMRX2 = inHeight-radiusX2;
	int16_t xPlusR = inX+inRadius;
	int16_t yPlusR = inY+inRadius;
	uint16_t	savedFGColor = mFGColor;
	uint16_t	fillColor = Calc565Color(inFillTint);
	mFGColor = fillColor;
	uint8_t	frameTint = DrawCircle(xPlusR,yPlusR, inRadius, inFrameOnly ? 1:0, eFullCircle, widthMRX2, heightMRX2);
	FillTintedRect(xPlusR,inY, widthMRX2, 1, frameTint);			// Top
	FillTintedRect(xPlusR,inY+inHeight-1, widthMRX2, 1, frameTint);	// Bottom
	FillTintedRect(inX, yPlusR, 1, heightMRX2, frameTint);			// Left
	FillTintedRect(inX+inWidth-1, yPlusR, 1, heightMRX2, frameTint);// Right
	mFGColor = savedFGColor;

	if (!inFrameOnly)
	{
		FillTintedRect(inX+1, yPlusR, inRadius-1, heightMRX2, inFillTint);
		FillTintedRect(inX+inWidth-inRadius, yPlusR, inRadius-1, heightMRX2, inFillTint);
		FillTintedRect(xPlusR, inY+1, widthMRX2, inHeight-2, inFillTint);
	}
	return(fillColor);
}

/********************************* DrawFrame **********************************/
void DisplayController::DrawFrame(
	uint16_t	inX,
	uint16_t	inY,
	uint16_t	inWidth,
	uint16_t	inHeight,
	uint16_t	inColor,
	uint8_t		inThickness)
{
	MoveTo(inY, inX);
	SetColumnRange(inWidth);
	FillPixels(inWidth * inThickness, inColor);
	MoveToRow(inY+inHeight-inThickness);
	SetColumnRange(inWidth);
	FillPixels(inWidth * inThickness, inColor);
	MoveToRow(inY+inThickness);
	SetColumnRange(inThickness);
	FillPixels((inHeight-(inThickness*2)) * inThickness, inColor);
	MoveToColumn(inX + inWidth - inThickness);
	SetColumnRange(inThickness);
	FillPixels((inHeight-(inThickness*2)) * inThickness, inColor);
}

/****************************** DrawTintedFrame *******************************/
void DisplayController::DrawTintedFrame(
	uint16_t	inX,
	uint16_t	inY,
	uint16_t	inWidth,
	uint16_t	inHeight,
	uint8_t		inTint,
	uint8_t		inThickness)
{
	DrawFrame(inX, inY, inWidth, inHeight, Calc565Color(inTint), inThickness);
}

/********************************* DrawFrame8 *********************************/
void DisplayController::DrawFrame8(
	const Rect8_t*	inRect,
	uint16_t		inColor,
	uint8_t			inThickness)
{
	DrawFrame(inRect->x, inRect->y, inRect->width, inRect->height, inColor, inThickness);
}

#if 1
/****************************** DrawCircle ******************************/
/*
*	Draws a whole or partial circle depending on inOctants.
*	This version of DrawCircle anti-aliases the circle using tint values.
*
*	inOctantXOffset & inOctantYOffset are used to offset the octants relative to
*	the circle center.  inOctantXOffset & inOctantYOffset are used when drawing
*	a rectangle with rounded corners so that DrawCircle is only called once.
*		NNE and ENE octants use inOctantXOffset
*		ESE and SSE octants use inOctantXOffset & inOctantYOffset
*		SSW and WSW octants use inOctantYOffset
*		WNW and NNW octants are not offset
*
*	The return value is used by DrawRoundedRect as the tint value of the one
*	pixel outer frame.  This value varies based on the radius.
*/
uint8_t DisplayController::DrawCircle(
	int16_t		inCenterX,
	int16_t		inCenterY,
	int16_t		inRadius,
	int16_t		inThickness,
	uint8_t		inOctants,
	int16_t		inOctantXOffset,
	int16_t		inOctantYOffset)
{
	int16_t	xOffset = inCenterX - inRadius;
	int16_t	yOffset = inCenterY - inRadius;
	/*
	*	Find the first column on the 45° line from the circle center that
	*	intersects the radius by solving the equation for a 45° right triangle:
	*	r² = x² + y², x=y, so r² = 2x² or x = r/√2
	*
	*	The first rounding error for the integer math below is at r = 816.
	*	The radius will be below 400 so this isn't a problem.
	*/
	uint32_t	firstCol = (inRadius*1000000)/141421;
	{
		uint32_t	firstColRem = firstCol % 10;
		firstCol /= 10;
		/*
		*	If the circle intersects the pixel firstCol+1 THEN
		*	increment firstCol.
		*/
		if (firstColRem)
		{
			firstCol++;
		}
	}
	int32_t		row = inRadius;
	int32_t	column;
	uint32_t	outerTintRadiusSquared = inRadius + 1;
	outerTintRadiusSquared *= outerTintRadiusSquared;
	
	uint32_t	radiusSquared = inRadius*inRadius;
	uint32_t	innerRadiusSquared;
	uint32_t	innerTintRadiusSquared;
	bool		fillEntireQuadrants = false;
	uint8_t		pattern[inRadius+2];
	
	if (inThickness &&
		inThickness < inRadius)
	{
		innerRadiusSquared = inRadius-inThickness+1;
		innerTintRadiusSquared = innerRadiusSquared-1;
		innerRadiusSquared *= innerRadiusSquared;
		innerTintRadiusSquared *= innerTintRadiusSquared;
	} else // else fill the entire circle
	{
		innerRadiusSquared = 0;
		innerTintRadiusSquared = 0;
		inThickness = 0;
		/*
		*	If not offsetting octants THEN
		*	Optimized fill is allowed.
		*/ 
		if (inOctantXOffset == 0 &&
			inOctantYOffset == 0)
		{
			/*
			*	fillEntireQuadrants means that there are no partial quadrants.
			*	i.e. there are no quadrants with only one octant requested.
			*/
			fillEntireQuadrants = ((inOctants & eEvenOctants) >> 1) == (inOctants & eOddOctants);
		}
	}

	uint8_t	roundRectTint = 0;
	uint8_t	tint = 0;
	for (uint32_t rowSquared = row*row; row; row--, rowSquared = row*row)
	{
		uint32_t	columnStart = firstCol;
		uint8_t*	patternPtr = pattern;
		for (column = columnStart; column; column--)
		{
			uint32_t	rcSquared = (column*column) + rowSquared;
			/*
			*	If this pixel is outside of the radius THEN
			*	update the starting column
			*/
			if (rcSquared > outerTintRadiusSquared)
			{
				columnStart = column-1 ;
				continue;
			/*
			*	If this pixel is between the outer tint radius and the radius THEN
			*	this pixel needs to be tinted.
			*/
			} else if (rcSquared > radiusSquared)
			{
				tint = map(rcSquared, outerTintRadiusSquared, radiusSquared, 0, 255);
			/*
			*	If this pixel is between the radius and the inner radius THEN
			*	this pixel is 100%
			*/
			} else if (rcSquared > innerRadiusSquared)
			{
				tint = 255;
			/*
			*	If this pixel is between the radius and the inner tint radius THEN
			*	this pixel needs to be tinted.
			*/
			} else if (rcSquared > innerTintRadiusSquared)
			{
				tint = map(rcSquared, innerRadiusSquared, innerTintRadiusSquared, 255, 0);
			/*
			*	Else just passed into empty interior THEN
			*	the row is complete.
			*/
			} else
			{
				break;
			}
			*(patternPtr++) = tint;
		}
		int32_t	patternLen = (int32_t)(patternPtr - pattern);
		if (patternLen)
		{
			if (!roundRectTint)
			{
				roundRectTint = tint;
			}
			/*
			*	Calc x and y of the NNW octant.  All of the other octants are
			*	based off/copies of the NNW octant.
			*/
			int16_t	x0 = inRadius-columnStart;
			int16_t	y0 = inRadius-row;
			patternPtr = pattern;

			/*
			*	If the 45° of the octant is hit, then limit the overlap between
			*	octants to one pixel.
			*/
			if (y0 > x0)
			{
				int32_t	patternInset = (y0-x0);
				patternLen -= patternInset;
				/*
				*	If there's anything to draw THEN
				*	clip x0 to y0
				*/
				if (patternLen)
				{
					x0 = y0;
					patternPtr += patternInset;
				/*
				*	Else, the circle is done, exit loop.
				*/
				} else
				{
					break;
				}
			}
			/*{
				for (uint32_t k = 0; k < patternLen; k++)
				{
					fprintf(stderr, " %hhu", patternPtr[k]);
				}
				fprintf(stderr, "\n");
			}*/

			int16_t	x1 = inRadius+column;
			int16_t	x2 = (inRadius*2)-1-y0;
			/*
			*	The variables below with the x or y prefix added, are offset
			*	either by the xOffset or the yOffset (hence the prefix)
			*/
			int16_t	xx0 = x0 + xOffset;
			int16_t	xx1 = x1 + xOffset + inOctantXOffset;
			int16_t	xx2 = x2 + xOffset + inOctantXOffset;
			int16_t	xy0 = y0 + xOffset;
			int16_t	yx0 = x0 + yOffset;
			int16_t	yx1 = x1 + yOffset + inOctantYOffset;
			int16_t	yx2 = x2 + yOffset + inOctantYOffset;
			int16_t	yy0 = y0 + yOffset;

			if (!fillEntireQuadrants ||
				x0 != y0 ||
				*patternPtr != 255)
			{
				if (inOctants & eNNEOctant)
				{
					CopyTintedPattern(xx1, yy0, patternPtr, patternLen, 1, false, true);
				}
				if (inOctants & eENEOctant)
				{
					CopyTintedPattern(xx2, yx0, patternPtr, patternLen, 1, true, false);
				}
				if (inOctants & eESEOctant)
				{
					CopyTintedPattern(xx2, yx1, patternPtr, patternLen, 1, true, true);
				}
				if (inOctants & eSSEOctant)
				{
					CopyTintedPattern(xx1, yx2, patternPtr, patternLen, 1, false, true);
				}
				if (inOctants & eSSWOctant)
				{
					CopyTintedPattern(xx0, yx2, patternPtr, patternLen, 1, false, false);
				}
				if (inOctants & eWSWOctant)
				{
					CopyTintedPattern(xy0, yx1, patternPtr, patternLen, 1, true, true);
				}
				if (inOctants & eWNWOctant)
				{
					CopyTintedPattern(xy0, yx0, patternPtr, patternLen, 1, true, false);
				}
				if (inOctants & eNNWOctant)
				{
					CopyTintedPattern(xx0, yy0, patternPtr, patternLen, 1, false, false);
				}
			/*
			*	Else at this point there are only rectangular areas representing
			*	the remainder of one or more quadrants. (100% fill optimization)
			*/
			} else
			{
				if (inOctants == eFullCircle)
				{
					FillRect(xx0, yy0, patternLen*2, patternLen*2, mFGColor);
				} else if (inOctants == eNorthHalf)
				{
					FillRect(xx0, yy0, patternLen*2, patternLen, mFGColor);
				} else if (inOctants == eSouthHalf)
				{
					FillRect(xy0, yx1, patternLen*2, patternLen, mFGColor);
				} else if (inOctants == eEastHalf)
				{
					FillRect(xx1, yy0, patternLen, patternLen*2, mFGColor);
				} else if (inOctants == eWestHalf)
				{
					FillRect(xx0, yy0, patternLen, patternLen*2, mFGColor);
				} else
				{
					if ((inOctants & eNEQuarter) == eNEQuarter)
					{
						FillRect(xx1, yy0, patternLen, patternLen, mFGColor);
					}
					if ((inOctants & eSEQuarter) == eSEQuarter)
					{
						FillRect(inCenterX, inCenterY, patternLen, patternLen, mFGColor);
					}
					if ((inOctants & eSWQuarter) == eSWQuarter)
					{
						FillRect(xy0, yx1, patternLen, patternLen, mFGColor);
					}
					if ((inOctants & eNWQuarter) == eNWQuarter)
					{
						FillRect(xx0, yy0, patternLen, patternLen, mFGColor);
					}
				}
				break;
			}
			if (patternLen <= 1)
			{
				break;
			}
		}
	}
	return(roundRectTint);
}

#else
/********************************* DrawCircle *********************************/
/*
*	Draws a whole or partial circle depending on inOctants.
*	This version does no anti-aliasing.  The circle is a solid color.
*/
uint8_t DisplayController::DrawCircle(
	int16_t	inCenterX,
	int16_t	inCenterY,
	int16_t	inRadius,
	int16_t	inThickness,
	uint8_t	inOctants,
	int16_t	inOctantXOffset,
	int16_t	inOctantYOffset)
{
	int16_t	xOffset = inCenterX - inRadius-1;
	int16_t	yOffset = inCenterY - inRadius-1;
	inRadius++;
	/*
	*	Find the first column on the 45° line from the circle center that
	*	intersects the radius by solving the equation for a 45° right triangle:
	*	r² = x² + y², x=y, so r² = 2x² or x = r/√2
	*
	*	The first rounding error for the integer math below is at r = 816.
	*	The radius will be below 400 so this isn't a problem.
	*/
	uint32_t	firstCol = ((inRadius*100000)+50000)/141421;

	int32_t		row = inRadius;
	uint32_t	column;
	uint32_t	firstRadiusSquared = inRadius*inRadius;
	uint32_t	lastRadiusSquared;
	bool		fillEntireQuadrants = false;
	
	if (inThickness &&
		inThickness < inRadius)
	{
		lastRadiusSquared = inRadius-inThickness;
		lastRadiusSquared *= lastRadiusSquared;
	} else // else fill the entire circle
	{
		lastRadiusSquared = 0;
		inThickness = 0;
		/*
		*	fillEntireQuadrants means that there are no partial quadrants.
		*	i.e. there are no quadrants with only one octant requested.
		*/
		fillEntireQuadrants = ((inOctants & eEvenOctants) >> 1) == (inOctants & eOddOctants);
	}

	for (uint32_t rowSquared = row*row; row; row--, rowSquared = row*row)
	{
		uint32_t	columnStart = firstCol;
		for (column = columnStart; column; column--)
		{
			uint32_t	rcSquared = (column*column) + rowSquared;
			/*
			*	If this pixel is outside of the radius THEN
			*	record the starting column
			*/
			if (rcSquared > firstRadiusSquared)
			{
				columnStart = column-1 ;
			/*
			*	Else if just passed into empty interior THEN
			*	the row is complete.
			*/
			} else if (rcSquared < lastRadiusSquared)
			{
				break;
			}
		}
		int16_t	runLen = columnStart-column;
		if (runLen)
		{
			/*
			*	Calc x and y of the NNW octant.  All of the other octants are
			*	based off/copies of the NNW octant.
			*/
			int16_t	x0 = inRadius-columnStart;
			int16_t	y0 = inRadius-row;

			/*
			*	If the 45° of the octant is hit, then limit the overlap between
			*	octants to one pixel.
			*/
			if (y0 > x0)
			{
				runLen -= (y0-x0);
				/*
				*	If there's anything to draw THEN
				*	clip x0 to y0
				*/
				if (runLen)
				{
					x0 = y0;
				/*
				*	Else, the circle is done, exit loop.
				*/
				} else
				{
					break;
				}
			}

			int16_t	x1 = inRadius+column;
			int16_t	x2 = (inRadius*2)-1-y0;
			/*
			*	The variables below with the x or y prefix added, are offset
			*	either by the xOffset or the yOffset (hence the prefix)
			*/
			int16_t	xx0 = x0 + xOffset;
			int16_t	xx1 = x1 + xOffset + inOctantXOffset;
			int16_t	xx2 = x2 + xOffset + inOctantXOffset;
			int16_t	xy0 = y0 + xOffset;
			int16_t	yx0 = x0 + yOffset;
			int16_t	yx1 = x1 + yOffset + inOctantYOffset;
			int16_t	yx2 = x2 + yOffset + inOctantYOffset;
			int16_t	yy0 = y0 + yOffset;
			
			
			if (!fillEntireQuadrants ||
				x0 != y0)
			{
				if (inOctants & eNNEOctant)
				{
					FillRect(xx1, yy0, runLen, 1, mFGColor);
				}
				if (inOctants & eENEOctant)
				{
					FillRect(xx2, yx0, 1, runLen, mFGColor);
				}
				if (inOctants & eESEOctant)
				{
					FillRect(xx2, yx1, 1, runLen, mFGColor);
				}
				if (inOctants & eSSEOctant)
				{
					FillRect(xx1, yx2, runLen, 1, mFGColor);
				}
				if (inOctants & eSSWOctant)
				{
					FillRect(xx0, yx2, runLen, 1, mFGColor);
				}
				if (inOctants & eWSWOctant)
				{
					FillRect(xy0, yx1, 1, runLen, mFGColor);
				}
				if (inOctants & eWNWOctant)
				{
					FillRect(xy0, yx0, 1, runLen, mFGColor);
				}
				if (inOctants & eNNWOctant)
				{
					FillRect(xx0, yy0, runLen, 1, mFGColor);
				}
			/*
			*	Else at this point there are only rectangular areas representing
			*	the remainder of one or more quadrants. (100% fill optimization)
			*/
			} else
			{
				if (inOctants == eFullCircle)
				{
					FillRect(xx0, yy0, runLen*2, runLen*2, mFGColor);
				} else if (inOctants == eNorthHalf)
				{
					FillRect(xx0, yy0, runLen*2, runLen, mFGColor);
				} else if (inOctants == eSouthHalf)
				{
					FillRect(xy0, yx1, runLen*2, runLen, mFGColor);
				} else if (inOctants == eEastHalf)
				{
					FillRect(xx1, yy0, runLen, runLen*2, mFGColor);
				} else if (inOctants == eWestHalf)
				{
					FillRect(xx0, yy0, runLen, runLen*2, mFGColor);
				} else
				{
					if ((inOctants & eNEQuarter) == eNEQuarter)
					{
						FillRect(xx1, yy0, runLen, runLen, mFGColor);
					}
					if ((inOctants & eSEQuarter) == eSEQuarter)
					{
						FillRect(inCenterX, inCenterY, runLen, runLen, mFGColor);
					}
					if ((inOctants & eSWQuarter) == eSWQuarter)
					{
						FillRect(xy0, yx1, runLen, runLen, mFGColor);
					}
					if ((inOctants & eNWQuarter) == eNWQuarter)
					{
						FillRect(xx0, yy0, runLen, runLen, mFGColor);
					}
				}
				break;
			}
			if (runLen <= 1)
			{
				break;
			}
		}
	}
	return(255);
}
#endif

/********************************** DrawLine **********************************/
/*
*	Draws a line of inThickness centered around the line made by the input
*	coordinates.
*
*	Any points after thickening that are off screen will be clipped.
*
*	The inUseMask option was added to allow for a 1 pixel animation without
*	having to erase the old line before drawing the new.
*/
void DisplayController::DrawLine(
	int16_t		inX0,
	int16_t		inY0,
	int16_t		inX1,
	int16_t		inY1,
	int16_t		inThickness,
	bool		inUseMask)
{
	if (inThickness == 0)
	{
		inThickness = 1;
	}

	int32_t	x;
	int32_t	y;
	/*
	*	If horizontal or vertical
	*/
	if (inY0 == inY1 ||
		inX0 == inX1)
	{
		/*
		*	Even thickness lines have a 50% tint value framed around the line.
		*	Odd thickness lines are solid.
		*/
		bool	useTint = (inThickness & 1) == 0;
		if (useTint)
		{
			inThickness++;
		}
		if (inX0 == inX1)
		{
			int32_t	height = inY1 - inY0;
			if (height < 0)
			{
				height = -height;
				y = inY1;
			} else
			{
				y = inY0;
			}
			height += inThickness;
			y -= (inThickness/2);
			x = inX0 - (inThickness/2);
			if (inUseMask)
			{
				DrawTintedFrame(x-2, y-2, inThickness+4, height+4, 0, 2);
			}
			if (useTint)
			{
				DrawTintedFrame(x, y, inThickness, height, 127, 1);
				y++;
				x++;
				inThickness -= 2;
				height -= 2;
			}
			FillTintedRect(x, y, inThickness, height, 255);
		} else
		{
			int32_t	width = inX1 - inX0;
			if (width < 0)
			{
				width = -width;
				x = inX1;
			} else
			{
				x = inX0;
			}
			width += inThickness;
			x -= (inThickness/2);
			y = inY0 - (inThickness/2);
			if (inUseMask)
			{
				DrawTintedFrame(x-2, y-2, width+4, inThickness+4, 0, 2);
			}
			if (useTint)
			{
				DrawTintedFrame(x, y, width, inThickness, 127, 1);
				x++;
				y++;
				inThickness -= 2;
				width -= 2;
			}
			FillTintedRect(x, y, width, inThickness, 255);
		}
	} else
	{
		/*
		*	Make the origin (0,0) and normalize the end point so that positive
		*	values relative to the origin are down and to the right:
		*
		*					 -2
		*					 -1
		*			 ...-2 -1 0  1  2...
		*					  1
		*					  2
		*/
		int32_t	dX = inX1 - inX0;
		int32_t dY = inY1 - inY0;

		/*
		*	Determine how to map the points back to the original octant.
		*/
		bool negateX =  dX < 0;		// Negate X to map back
		if (negateX)
		{
			dX = -dX;
		}
		bool	negateY = dY < 0;	// Negate Y to map back
		if (negateY)
		{
			dY = -dY;
		}
		bool swapXY = dX > dY;		// Swap XY to map back
		if (swapXY)
		{
			SwapInt32(dX, dY);
			if (negateY)
			{
				negateY = false;
				negateX = !negateX;
				SwapInt16(inX0,inX1);
				SwapInt16(inY0,inY1);
			}
		} else if (negateX)
		{
			negateX = false;
			negateY = !negateY;
			SwapInt16(inX0,inX1);
			SwapInt16(inY0,inY1);
		}
		/*
		*	To map back to the original octant, the above conditions are applied
		*	in reverse order, i.e. swap, negate, add inX0, inY0.
		*
		*	Note that this routine forces the math for all lines to be
		*	within a single octant, a 45° range.
		*
		*	The starting point is the thickness + anti-alias thickness + any
		*	mask thickness, relative to the zero origin.  This covers the worse
		*	case when the slope approaches 45°.
		*
		*	The generated patterns for the first half of the line are mirrored
		*	to the 2nd half.  This can be done because the line must start and
		*	end on a pixel (where the other pixels are just close to the line.)
		*/
		const uint32_t	kAntiAliasThickness = 2;
		const uint32_t	kHalfUnitMultiplier = 5000;
		int32_t	thicknessPlusAM = inThickness+kAntiAliasThickness+(inUseMask ? 2 : 0);
		int32_t	pixelLimit = (inThickness+kAntiAliasThickness)*kHalfUnitMultiplier;
		int32_t	maskPixelLimit = inUseMask ? (thicknessPlusAM*kHalfUnitMultiplier) : 0;
		int32_t	tintAbove = pixelLimit-(kHalfUnitMultiplier*kAntiAliasThickness);
		int32_t	klugeXAdjustment = 0;
		int32_t	xStep;
		int32_t	yStep;
		{
			double	m = (double)dX/dY;
			/*
			*	The "klugeXAdjustment" is a value added to X for lines with a
			*	slope between 22.5° and 45°, to adjust for a 1 pixel shift.
			*	I'm not sure why the shift occurs, but it is repeatable
			*	regardless of the line thickness and length.
			*/
			if (m > 0.4142)
			{
				klugeXAdjustment = 1;
			}
			xStep = (1.0/sqrt((m*m)+1))*10000;
			m = 1/m;
			yStep = (1.0/sqrt((m*m)+1))*10000;
		}
		int32_t	initialX = -thicknessPlusAM;
		int32_t	mirrorX = dX+thicknessPlusAM;
		int32_t	mirrorY = dY+thicknessPlusAM;
		int32_t	halfDy = (dY+1)/2;
		int32_t	distance = (yStep*thicknessPlusAM) - (xStep*thicknessPlusAM);
		int32_t	distanceInv = (xStep*thicknessPlusAM) + (yStep*thicknessPlusAM);
		int32_t	tint = 0;
		/*
		*	At near 45° the max pattern width is twice the thickness.
		*	If using a mask +2.
		*/
		uint8_t	pattern[(thicknessPlusAM*2)+2];
		uint8_t*	patternStart = pattern;
		if (inUseMask)
		{
			pattern[0] = 0;
			pattern[1] = 0;
			patternStart+=2;
		}
		int32_t	lastX = 0;
		int32_t	lastXM = 0;
		
		for (y = -thicknessPlusAM; y <= halfDy; y++)
		{
			bool	crossedLine = false;
			uint8_t*	patternPtr = patternStart;
			int32_t	xyInvDistance = distanceInv;
			int32_t	xyDistance = distance;
			int32_t	insetX = 0;
			for (; true; xyDistance+=xStep, xyInvDistance-=yStep)
			{
				int32_t	absPixelDistance;	// Distance from line
				/*
				*	If this pixel is to the left of the line...
				*/
				if (xyDistance < 0)
				{
					absPixelDistance = -xyDistance;
					/*
					*	If this pixel should be ignored
					*/
					if (absPixelDistance >= pixelLimit &&
						xyInvDistance <= 0)
					{
						distance+=xStep;	// Don't look at this column again
						initialX++;
						mirrorX--;
						continue;
					}
				} else
				{
					/*
					*	If this pixel is the first pixel to the right of the
					*	line (i.e. just crossed over the line) THEN
					*	the distance to the line is measured from the top right
					*	of the pixel.
					*/
					if (!crossedLine)
					{
						crossedLine = true;
						xyDistance+=yStep;
						xyDistance+=xStep;
					}
					if (xyDistance < pixelLimit)
					{
						absPixelDistance = xyDistance;
					} else
					{
						break;
					}
				}
				/*
				*	If this pixel is above the line perpendicular at the origin
				*	to the line being drawn AND
				*	the inverse distance is larger than the xyDistance THEN
				*	this pixel is in the area of the end cap so the most distant
				*	distance is used.
				*/
				if (xyInvDistance > 0 &&
					xyInvDistance > absPixelDistance)
				{
					absPixelDistance = xyInvDistance; // Purpendicular distance
				}
				if (absPixelDistance > tintAbove)
				{
					/*
					*	If this pixel needs to be tinted...
					*/
					if (absPixelDistance < pixelLimit)
					{
						tint = map(absPixelDistance, tintAbove, pixelLimit, 255, 0);
					} else if (absPixelDistance < maskPixelLimit)
					{
						tint = 0;
					/*
					*	Else, this pixel should be ignored.
					*/
					} else
					{
						/*
						*	If the pattern hasn't started yet THEN
						*	inset X by one to account for the skipped pixel.
						*/
						if (patternPtr == patternStart)
						{
							insetX++;
							continue;
						/*
						*	Else we're at the end of the row.  Exit the loop.
						*/
						} else
						{
							break;
						}
					}
				} else
				{
					tint = 255; // color is 100%
				}
				*(patternPtr++) = tint;
			}
			distance-=yStep;
			distanceInv-=xStep;
			/*
			*	Map x and y to the original octant then draw the tint pattern.
			*/
			{
				int32_t	patternLen = (int32_t)(patternPtr - patternStart);
				if (patternLen)
				{
					int32_t	oY = y;
					int32_t	oX = initialX + insetX + klugeXAdjustment;
					if (inUseMask)
					{
						*(patternPtr++) = 0;
						*patternPtr = 0;
						patternLen+=4;
						oX-=2;
					}
				
					if (swapXY)
					{
						SwapInt32(oX, oY);
					}
					if (negateX)
					{
						oX = -oX;
					}
					if (negateY)
					{
						oY = -oY;
					}
					oX += inX0;
					oY += inY0;
					uint32_t patternInset;
					int32_t	clippedPatternLen = patternLen;
					int32_t	reps = 1;
					if (swapXY)
					{
						ClipX(oX, reps);
						patternInset = ClipY(oY, clippedPatternLen);
					} else
					{
						patternInset = ClipX(oX, clippedPatternLen);
						ClipY(oY, reps);
					}
					if (reps && clippedPatternLen)
					{
						lastX = oX;
						CopyTintedPattern(oX, oY, &pattern[patternInset], clippedPatternLen, 1, swapXY, false);
					}
					if (y < mirrorY)
					{
						reps = 1;
						oY = mirrorY;
						oX = mirrorX - patternLen - insetX;
						if (inUseMask)
						{
							oX+=2;
						}
						if (swapXY)
						{
							SwapInt32(oX, oY);
						}
						if (negateX)
						{
							oX = -oX;
						}
						if (negateY)
						{
							oY = -oY;
						}
						oX += inX0;
						oY += inY0;
						if (swapXY)
						{
							ClipX(oX, reps);
							patternInset = ClipY(oY, patternLen);
						} else
						{
							patternInset = ClipX(oX, patternLen);
							ClipY(oY, reps);
						}
						if (reps && patternLen)
						{
							lastXM = oX;
							CopyTintedPattern(oX, oY, &pattern[patternInset], patternLen, 1, swapXY, true);
						}
					}
				}
				mirrorY--;
			}
		}
	}
//	FillRect(inX0, inY0, 1, 1, 0x001F);
//	FillRect(inX1, inY1, 1, 1, 0x07E0);
}

/***************************** CopyTintedPattern ******************************/
/*
*	Added as an optimization for drawing anti-aliased lines.  The pattern is
*	copied inReps times in either the horizontal or vertical direction.
*
*	The foreground and background colors need to be set using SetFGColor and
*	SetBGColor prior to calling this routine.
*
*	inTintPattern is an array of tint values (0 to 255.)  For color displays,
*	the tints are converted to colors using the foreground and background
*	colors. 8 bit Grayscale displays could use the tint values directly. 
*	Monochrome displays could use the tint to determine whether to set the pixel
*	or not. inHorizontal is the copy direction, by column or by row. When
*	inReverseOrder is true, the tint conversion to color values starts at offset
*	inPatternLen-1 and ends at offset 0.
*/
void DisplayController::CopyTintedPattern(
	uint16_t		inX,
	uint16_t		inY,
	const uint8_t*	inTintPattern,
	uint16_t		inPatternLen,
	uint16_t		inReps,
	bool			inVertical,
	bool			inReverseOrder)
{
	// See TFT_ST77XX::CopyTintedPattern for an implementation example
}

/********************************* DrawFrameP *********************************/
/*void DisplayController::DrawFrameP(
	const Rect8_t*	inRect,
	uint16_t		inColor,
	uint8_t			inThickness)
{
	Rect8_t	rect8;
	memcpy_P(&rect8, inRect, sizeof(Rect8_t));
	DrawFrame(&rect8, inColor, inThickness);
}*/

/******************************* SetColumnRange *******************************/
void DisplayController::SetColumnRange(
	uint16_t	inRelativeWidth)
{
	SetColumnRange(mColumn, mColumn + inRelativeWidth-1);
}

/******************************* SetRowRange *******************************/
void DisplayController::SetRowRange(
	uint16_t	inRelativeHeight)
{
	SetRowRange(mRow, mRow + inRelativeHeight-1);
}

/***************************** FillTillEndColumn ******************************/
void DisplayController::FillTillEndColumn(
	uint16_t	inRows,
	uint16_t	inFillColor)
{
	FillBlock(inRows, mColumns, inFillColor);
}

/****************************** StreamCopyBlock *******************************/
bool DisplayController::StreamCopyBlock(
	DataStream*	inDataStream,
	uint16_t		inRows,
	uint16_t		inColumns)
{
	bool	success = WillFit(inRows, inColumns);
	if (success)
	{
		uint16_t	pixelsToCopy = inRows * inColumns;
		if (pixelsToCopy)
		{
			if (mAddressingMode == eHorizontal)
			{
				SetColumnRange(inColumns);
				// The column index will wrap back to the starting point.
				// The page won't so it needs to be reset.
				StreamCopy(inDataStream, pixelsToCopy);
				SetColumnRange(0, mColumns-1);	// Remove the column range clipping
				MoveToRow(mRow);	// Leave the page unchanged
				MoveColumnBy(inColumns); // Advance by inColumns (or wrap to zero if at or past end)
			} else
			{
				MoveToRow(mRow);	// Leave the page unchanged
				SetRowRange(inRows);
				// The row index will wrap back to the starting point.
				// The column won't so it needs to be reset.
				StreamCopy(inDataStream, pixelsToCopy);
				MoveToRow(mRow);	// Leave the page unchanged
				MoveColumnBy(inColumns); // Advance by inColumns (or wrap to zero if at or past end)
			}
		}
	}
	
	return(success);
}

/******************************** Calc565Color ********************************/
uint16_t DisplayController::Calc565Color(
	uint8_t		inTint)
{
	return(Calc565Color(mFGColor, mBGColor, inTint));
}

/******************************** Calc565Color ********************************/
uint16_t DisplayController::Calc565Color(
	uint16_t	inFG,
	uint16_t	inBG,
	uint8_t		inTint)
{
	uint16_t color;
	if (inTint == 0xFF)
	{
		color = inFG;
	} else if (inTint == 0)
	{
		color = inBG;
	} else if (inBG != 0)
	{
		uint8_t	bgTint = 255 - inTint;
		color = ((((inFG >> 11) * inTint + (inBG >> 11) * bgTint) / 255) << 11) +
				(((((inFG >> 5) & 0x3F) * inTint + ((inBG >> 5) & 0x3F) * bgTint) / 255) << 5) +
				(((inFG & 0x1F) * inTint + (inBG & 0x1F) * bgTint) / 255);
	} else
	{
		color = ((((inFG >> 11) * inTint) / 255) << 11) +
				(((((inFG >> 5) & 0x3F) * inTint) / 255) << 5) +
				((((inFG & 0x1F) * inTint) / 255));
	}
	return(color);
}

#if 0
/**************************** DrawAlignmentPoints *****************************/
/*
*	Used to draw 9 points inset by 20 pixels:	*	*	*
*												*	*	*
*												*	*	*
*
*	See XPT2046::Align() for use case.
*/
void DisplayController::DrawAlignmentPoints(void)
{
	const uint16_t	kInset = 20;
	uint16_t	iX = (mColumns - (kInset*2))/2;
	uint16_t	iY = (mRows - (kInset*2))/2;
	for (uint16_t y = kInset; y < mRows; y+= iY)
	{
		for (uint16_t x = kInset; x < mColumns; x+= iX)
		{
			FillRect(x, y, 2, 2, 0x1F);
		}
	}
}
#endif

#ifdef __MACH__
/************************************ map *************************************/
int32_t DisplayController::map(
	int32_t x,
	int32_t inMin,
	int32_t inMax,
	int32_t outMin,
	int32_t outMax)
{
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}
#endif

