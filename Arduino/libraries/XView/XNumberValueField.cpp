/*
*	XNumberValueField.cpp, Copyright Jonathan Mackey 2023
*	The number value field associated with a stepper.
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

#include "XNumberValueField.h"
#include "DisplayController.h"

/****************************** XNumberValueField *****************************/
XNumberValueField::XNumberValueField(
	int16_t					inX,		// Not used when aligned to stepper
	int16_t					inY,		// Not used when aligned to stepper
	uint16_t				inWidth,
	uint16_t				inTag,
	XView*					inNextView,
	XFont::Font*			inFont,
	int32_t					inValue,
	int32_t					inMaxValue,
	int32_t					inMinValue,
	uint32_t				inIncrement,
	bool					inValueWraps,
	bool					inRoundToIncrement,
	ValueFormatterPtr		inValueFormatter,
	uint16_t				inFGColor,
	uint16_t				inBGColor,
	XFont::ETextAlignment	inTextAlignment)
	: XValueField(inX, inY, inWidth, inTag, inNextView, inFont,
	  inValue, inFGColor, inBGColor, inTextAlignment),
	  mValueFormatter(inValueFormatter), mIncrement(inIncrement),
	  mMinimum(inMinValue), mMaximum(inMaxValue), mValueWraps(inValueWraps),
	  mRoundToIncrement(inRoundToIncrement)
{
}

/********************************** DrawSelf **********************************/
void XNumberValueField::DrawSelf(void)
{
	XFont*	xFont = MakeFontCurrent();
	if (xFont && mVisible)
	{
		uint16_t	textColor = mEnabled ? mFGColor :
						DisplayController::Calc565Color(mFGColor, 0, 184);
		xFont->SetTextColor(textColor);
		xFont->SetBGTextColor(mBGColor);
		int16_t	x = 0;
		int16_t	y = 0;
		LocalToGlobal(x, y);
		xFont->DrawAligned(mValueString, x, y, mWidth, mTextAlignment, true);
	}
}

/******************************** ValueIsValid ********************************/
bool XNumberValueField::ValueIsValid(
	int32_t	inValue)
{
	return(inValue >= mMinimum && inValue <= mMaximum);
}

/******************************* IncrementValue *******************************/
bool XNumberValueField::IncrementValue(void)
{
	if (mRoundToIncrement && mIncrement)
	{
		mValue -= (mValue % mIncrement);
	}
	int32_t	newValue = mValue + mIncrement;
	if (newValue > mMaximum)
	{
		if (mValueWraps)
		{
			newValue = mMinimum;
		} else
		{
			newValue = mMaximum;
		}
	}
	return(SetValue(newValue, true));
}

/******************************* DecrementValue *******************************/
bool XNumberValueField::DecrementValue(void)
{
	if (mRoundToIncrement && mIncrement)
	{
		mValue -= (mValue % mIncrement);
	}
	int32_t	newValue = mValue - mIncrement;
	if (newValue < mMinimum)
	{
		if (mValueWraps)
		{
			newValue = mMaximum;
		} else
		{
			newValue = mMinimum;
		}
	}
	return(SetValue(newValue, true));
}

/**************************** UpdateStringForValue ****************************/
void XNumberValueField::UpdateStringForValue(void)
{
	if (mValueFormatter)
	{
		mValueFormatter(mValue, mValueString);
	} else
	{
		ValueFormatter::Int32ToString(mValue, mValueString);
	}
}

/**************************** OverrideValueString *****************************/
/*
*	OverrideValueString overrides the calculated value normally set by
*	UpdateStringForValue(). This value will be shown until UpdateStringForValue
*	is called via SetValue() or ValueChanged()
*	inString must be less than 14 characters.
*/
void XNumberValueField::OverrideValueString(
	const char*	inString,
	bool		inUpdate)
{
	strncpy(mValueString, inString, sizeof(mValueString));
	mValue = 0x6423FFF0;	// Set some random number so that setting any value
							// will cause the displayed value to update
							// mValueString.
	if (inUpdate)
	{
		DrawSelf();
	}
}

