/*
*	FilterStatusGauge.cpp, Copyright Jonathan Mackey 2023
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
#include "FilterStatusGauge.h"
#include "DisplayController.h"
#ifdef __MACH__
	#define map DisplayController::map
#endif

const int32_t	FilterStatusGauge::kIndicatorGap = 8;
const int32_t	FilterStatusGauge::kIndicatorThickness = 5;
const int32_t	FilterStatusGauge::kInfoFrameThickness = 4;

/***************************** FilterStatusGauge ******************************/
FilterStatusGauge::FilterStatusGauge(
	int16_t			inX,
	int16_t			inY,
	uint16_t		inWidth,
	uint16_t		inHeight,
	uint16_t		inTag,
	XView*			inNextView,
	XView*			inSubViews,
	XFont::Font*	inFont,
	uint16_t		inGaugeThickness,
	uint16_t		inInfoFrameRadius)
: XColoredView(inX, inY, inWidth, inHeight, inTag, inNextView, inSubViews,
	nullptr, false),	// false = not visible
  mAnimationPeriod(30), mFont(inFont),
  mGaugeThickness(inGaugeThickness), mInfoFrameRadius(inInfoFrameRadius),
  mRadius(inHeight), mPos(0), mIndicatorPos(0), mIndicatorPercentage(0),
  mMin(0), mMax(100)
{
	/*
	*	Used by DrawGauge to make sure the arc and its origin are visible...
	*/
	mGaugeWidth = ((((mRadius-mGaugeThickness-kIndicatorGap)*100000)+50000)/141421) * 2;

	if (mFont)
	{
		XFont*	xFont = mFont->MakeCurrent();
		xFont->LoadGlyph('%');
		uint16_t	percentCharWidth = xFont->Glyph().advanceX;
		xFont->LoadGlyph('0');
		mDigitWidth = xFont->Glyph().advanceX;

		mInfoStrTop = mHeight - xFont->GetFontHeader().height - 4;
		mInfoStrLeft = (mWidth/2) - (((mDigitWidth*3)+percentCharWidth)/2);
	}
	mAnimationPeriod.Start();
}

/********************************* SetMinMax **********************************/
/*
*	The min and max are used to map the value to a number from 0 to mGaugeWidth.
*/
void FilterStatusGauge::SetMinMax(
	int32_t	inMin,
	int32_t	inMax)
{
	mMin = inMin;
	mMax = inMax;
}

/********************************** SetValue **********************************/
void FilterStatusGauge::SetValue(
	int32_t	inValue)
{
	mPos = map(inValue, mMin, mMax, 0, mGaugeWidth);
	if (mPos < 0)
	{
		mPos = 0;
	} else if (mPos > mGaugeWidth)
	{
		mPos = mGaugeWidth;
	}
	if (!mVisible)
	{
		mIndicatorPos = mPos;
	}
}

/*********************************** Update ***********************************/
void FilterStatusGauge::Update(void)
{
	/*
	*	If the indicator needs to move THEN
	*	redraw the indicator.
	*
	*	The indicator line is drawn with a 1-2 pixel mask around it depending on
	*	the angle.  The line can only move by one pixel per call.  This allows
	*	the line to move without having to erase the previous line.  The
	*	indicator will always try to match the position mPos but it may take
	*	several Update calls to accomplish this.  The nice side effect of
	*	drawing the line like this is it eliminates any spikes in the position.
	*/
	if (mVisible &&
		mAnimationPeriod.Passed() &&
		mIndicatorPos != mPos)
	{
		mAnimationPeriod.Start();
		if (mIndicatorPos < mPos)
		{
			mIndicatorPos++;
		} else
		{
			mIndicatorPos--;
		}
		XFont*	xFont = mFont->MakeCurrent();
		DisplayController*	display = xFont->GetDisplay();
		/*
		*	Draw the indicator line.  The indicator is drawn between the arc of
		*	the multi color gauge and the half circle that surrounds the filter
		*	loading percentage text.  The origin of the indicator line is the
		*	same as the color gauge and half circle.
		*/
		{
			int32_t	halfDisplayWidth = mWidth/2;
			int32_t	halfGaugeWidth = mGaugeWidth/2;
			int32_t	innerRadius = mRadius - mGaugeThickness - kIndicatorGap;
			int32_t	x1 = halfDisplayWidth - halfGaugeWidth + mIndicatorPos;
			int32_t	xToOrigin = halfGaugeWidth - mIndicatorPos;
			int32_t	y1 = xToOrigin ? sqrt((innerRadius*innerRadius)-(xToOrigin*xToOrigin)) : innerRadius;
			int32_t	r2m = ((mInfoFrameRadius + 10)*100000)/innerRadius;
			int32_t	x0 = halfDisplayWidth-((r2m*xToOrigin)/100000);
			int32_t	y0 = mHeight - ((r2m*y1)/100000);
			display->SetFGColor(eIndicatorColor);
			display->SetBGColor(0);
			display->DrawLine(x0, y0, x1, mHeight-y1, kIndicatorThickness, true);
		}
		/*
		*	Calculate the displayed indicator position as a percentage of the
		*	gauge range.
		*	Update the displayed percentage as needed.
		*/
		int32_t	indicatorPercentage = map(mIndicatorPos, 0, mGaugeWidth, 0, 100);
		if (indicatorPercentage != mIndicatorPercentage)
		{
			mIndicatorPercentage = indicatorPercentage;
			char percentageStr[10];
			percentageStr[9] = 0;
			char*	strPtr = &percentageStr[8];
			do
			{
				*(strPtr--) = (indicatorPercentage % 10) + '0';
				indicatorPercentage /= 10;
			} while (indicatorPercentage);
			strPtr++;
			display->MoveTo(mInfoStrTop, mInfoStrLeft);
			uint16_t	strLeft =  mInfoStrLeft + (mDigitWidth*(3-(&percentageStr[9]-strPtr)));
			if (strLeft > mInfoStrLeft)
			{
				xFont->EraseTillColumn(strLeft);
			}
			xFont->SetTextColor(XFont::eWhite);
			xFont->SetBGTextColor(XFont::eBlack);
			xFont->DrawStr(strPtr);
		}
	}
}

/********************************** DrawSelf **********************************/
void FilterStatusGauge::DrawSelf(void)
{
	XFont*	xFont = mFont->MakeCurrent();
	DisplayController*	display = xFont->GetDisplay();
	display->SetFGColor(eIndicatorColor);
	display->SetBGColor(0);
	DrawGauge();
	display->DrawCircle(mWidth/2, mRadius/*+mTop*/,
		mInfoFrameRadius, kInfoFrameThickness, DisplayController::eNorthHalf);
	display->MoveTo(mInfoStrTop, mInfoStrLeft + (mDigitWidth*3));
	xFont->SetTextColor(XFont::eWhite);
	xFont->SetBGTextColor(XFont::eBlack);
	xFont->DrawStr("%");
	if (mIndicatorPos == mPos)
	{
		mIndicatorPos = mPos-1;	// To force a redraw
	}
	mIndicatorPercentage = -1;	// To force a redraw
	Update();
}

/********************************* DrawGauge **********************************/
/*
*	Draws a 90° arc of mRadius x mGaugeThickness at 0,0.  The left half of the
*	arc goes from green to yellow, the right half goes from yellow to red.
*
*	This routine uses all integer math.
*/
void FilterStatusGauge::DrawGauge(void)
{
	XFont*	xFont = mFont->MakeCurrent();
	DisplayController*	display = xFont->GetDisplay();

	/*
	*	Find the first column on the 45° line from the circle center that
	*	intersects the radius by solving the equation for a 45° right triangle:
	*	r² = x² + y², x=y, so r² = 2x² or x = r/√2
	*/
	/*
	*	The first rounding error for the integer math below is at r = 816.
	*	The radius will be below 400 so this isn't a problem.
	*/
	uint32_t	transLineLen = ((mRadius*100000)+50000)/141421;
	
	
	uint32_t	gaugeWidth = transLineLen*2;
	mLeft = (mWidth/2) - transLineLen;

	/*
	*	Columns before the intersection of the radius at 45° are skipped.
	*
	*	If this isn't done you would have a seemingly random amount of dead
	*	space to the left of the arc.
	*/
	uint32_t	columnsToSkip = mRadius - transLineLen;

	uint16_t	pixelLine[gaugeWidth+1];
	uint16_t*	pixellineEnd = &pixelLine[gaugeWidth];
	
	/*
	*	Generate the left and right color gradients
	*/
	uint16_t	leftTransLine[transLineLen];
	uint16_t	rightTransLine[transLineLen];
	GenerateTransitionLine(eCenterColor, eStartColor, transLineLen, leftTransLine);
	GenerateTransitionLine(eCenterColor, eEndColor, transLineLen, rightTransLine);

	uint32_t	dispInset = columnsToSkip ? columnsToSkip-1 : 0;
	uint32_t	row = mRadius;
	uint32_t	column;
	uint32_t	padLeft = 0;
	uint32_t	padRight = 0;
	uint32_t	firstRadiusSquared = mRadius*mRadius;
	uint32_t	lastRadiusSquared = mRadius-mGaugeThickness;
	lastRadiusSquared *= lastRadiusSquared;

	for (uint32_t rowSquared = row*row; rowSquared > 1; row--, rowSquared = row*row)
	{
		uint16_t*	leftPixelPtr = pixelLine;
		uint16_t*	rightPixelPtr = pixellineEnd;
		
		for (column = row-columnsToSkip; column; column--)
		{
			uint32_t	rcSquared = (column*column) + rowSquared;
			/*
			*	If this pixel is within the radius + thickness THEN
			*	load this pixel based on its position within arc.
			*/
			if (rcSquared <= firstRadiusSquared &&
				rcSquared >= lastRadiusSquared)
			{
				uint32_t colorIndex = map(column, 0, row, 0, transLineLen);
				*(leftPixelPtr++) = leftTransLine[colorIndex];
				*(rightPixelPtr--) = rightTransLine[colorIndex];
			/*
			*	Else if this pixel is outside of the radius THEN
			*	account for the empty space (which won't be drawn.)
			*/
			} else if (rcSquared > firstRadiusSquared)
			{
				padLeft++;
			/*
			*	Else if just passed into empty arc interior THEN
			*	pad till start of other interior side.
			*/
			} else if (rcSquared < lastRadiusSquared)
			{
				padRight++;
				if (column != row)
				{
					padRight += (column*2);
				}
				column++;
				break;
			}
		}
		/*
		*	Draw the left half of the arc row...
		*/
		{
			if (column == 0)
			{
				*(leftPixelPtr++) = eCenterColor;
			}
			uint16_t	thisLineLen = leftPixelPtr-pixelLine;
			if (thisLineLen)
			{
				if (row + dispInset < mRadius)
				{
					padLeft += (mRadius - row - dispInset);
				}
				display->MoveTo(/*mTop + */mRadius - row, mLeft + padLeft);
				display->SetColumnRange(thisLineLen);
				display->CopyPixels(pixelLine, thisLineLen);
				padRight += thisLineLen;
			} else
			{
				break;
			}
		}
		/*
		*	Draw the right half of the arc row...
		*/
		{
			uint16_t	thisLineLen = pixellineEnd-rightPixelPtr;
			if (thisLineLen)
			{
				display->MoveBy(0, padRight);
				display->SetColumnRange(thisLineLen);
				display->CopyPixels(rightPixelPtr+1, thisLineLen);
			}
		}
		padLeft = 0;
		padRight = 0;
		
		if (columnsToSkip)
		{
			columnsToSkip--;
		}
	}
}

/*************************** GenerateTransitionLine ***************************/
void FilterStatusGauge::GenerateTransitionLine(
	uint16_t	inFromColor,
	uint16_t	inToColor,
	uint16_t	inNumSteps,
	uint16_t*	outLine)
{
	int16_t	stepValue, value, remInc;
	uint16_t	remAccumulator, remValue;
	uint16_t*	outLinePtr;
	const static uint16_t	mask565[] = {0x1F, 0x7E0, 0xF800};
	const static uint8_t	shiftAmt[] = {0, 5, 11};
	for (uint8_t sep = 0; sep < 3; sep++)
	{
		uint16_t	fromColor = inFromColor & mask565[sep];
		uint16_t	toColor = inToColor & mask565[sep];
		outLinePtr = outLine;
		remAccumulator = 0;
		if (sep)
		{
			*outLinePtr |= fromColor;
			fromColor >>= shiftAmt[sep];
			toColor >>= shiftAmt[sep];
		} else
		{
			*outLinePtr = fromColor;
		}
		outLinePtr++;
		value = fromColor;
		stepValue = toColor - fromColor;
		remValue = (int16_t)stepValue % inNumSteps;
		if (remValue & 0x80)
		{
			remValue = -remValue;
			remInc = -1;
		} else
		{
			remInc = 1;
		}
		stepValue = (int16_t)stepValue/inNumSteps;
		for (uint16_t step = 0; step < inNumSteps; step++)
		{
			value += stepValue;
			remAccumulator += remValue;
			if (remAccumulator >= inNumSteps)
			{
				value += remInc;
				remAccumulator -= inNumSteps;
			}
			if (sep)
			{
				*outLinePtr |= (((uint16_t)value) << shiftAmt[sep]);
			} else
			{
				*outLinePtr = value;
			}
			outLinePtr++;
		}
	}
}

#if 0
/****************************** TransColorAtPos *******************************/
uint16_t FilterStatusGauge::TransColorAtPos(
	uint16_t	inPosition)
{
	uint16_t	transColor;
	uint16_t	halfGaugeWidth = mGaugeWidth/2;
	if (inPosition < halfGaugeWidth)
	{
		transColor = CalcTransitionColor(eStartColor, eCenterColor, halfGaugeWidth, inPosition);
	} else
	{
		transColor = CalcTransitionColor(eCenterColor, eEndColor, halfGaugeWidth, inPosition - halfGaugeWidth);
	}
	return(transColor);
}

/**************************** CalcTransitionColor *****************************/
uint16_t FilterStatusGauge::CalcTransitionColor(
	uint16_t	inFromColor,
	uint16_t	inToColor,
	uint16_t	inNumSteps,
	uint16_t	inStep)
{
	uint16_t	transColor = 0;
	if (inStep)
	{
		if (inStep <= inNumSteps)
		{
			int8_t	stepValue, remValue, value;
			const static uint16_t	mask565[] = {0x1F, 0x7E0, 0xF800};
			const static uint8_t	shiftAmt[] = {0, 5, 11};
			for (uint8_t sep = 0; sep < 3; sep++)
			{
				uint16_t	fromColor = inFromColor & mask565[sep];
				uint16_t	toColor = inToColor & mask565[sep];
				if (sep)
				{
					fromColor >>= shiftAmt[sep];
					toColor >>= shiftAmt[sep];
				}
				stepValue = toColor - fromColor;
				remValue = stepValue % inNumSteps;
				stepValue /= inNumSteps;
				value = fromColor;
				value += ((stepValue*inStep) + ((remValue*inStep)/inNumSteps));
				if (sep)
				{
					transColor |= (((uint16_t)value) << shiftAmt[sep]);
				} else
				{
					transColor = value;
				}
			}
		} else
		{
			transColor = inToColor;
		}
	} else
	{
		transColor = inFromColor;
	}
	return(transColor);
}
#endif