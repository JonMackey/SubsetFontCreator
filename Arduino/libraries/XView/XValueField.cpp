/*
*	XValueField.cpp, Copyright Jonathan Mackey 2023
*	Base class for a stepper value field.
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

#include "XValueField.h"
#include "XStepper.h"

/******************************** XValueField *********************************/
XValueField::XValueField(
	int16_t					inX,		// Not used when aligned to stepper
	int16_t					inY,		// Not used when aligned to stepper
	uint16_t				inWidth,	// May not be used for certain subclasses
	//uint16_t				inHeight,	// Not used, based on font
	uint16_t				inTag,
	XView*					inNextView,
	XFont::Font*			inFont,
	int32_t					inValue,
	uint16_t				inFGColor,
	uint16_t				inBGColor,
	XFont::ETextAlignment	inTextAlignment)
	: XControl(inX, inY, inWidth, 20, inTag, inNextView),
	  mStepper(nullptr), mFont(inFont), mValue(inValue),
	  mBGColor(inBGColor), mFGColor(inFGColor),mTextAlignment(inTextAlignment)
{
}

/*
*	Note that if no stepper is assigned, this view's height is zero.  If this
*	view needs to be dynamically hidden, it needs a height.  Call SetHeight()
*	somewhere during startup.  It was too much of a PITA to go back and
*	reinstate inHeight in the constructor.
*/

/******************************** SetXStepper *********************************/
XFont* XValueField::SetXStepper(
	XStepper*	inXStepper)
{
	mStepper = inXStepper;
	XFont*	xFont = MakeFontCurrent();
	if (inXStepper &&
		xFont)
	{
		int16_t	stepperHeight = inXStepper->Height();
		mHeight = xFont->GetFontHeader().height;

		/*
		*	If the height is zero THEN
		*	calculate the height based on the font.
		*	Note that this only works on fonts created with the minimized height
		*	option set.  If the font has too few glyphs, the result is
		*	unpredictable.
		*/
		if (stepperHeight == 0)
		{
			stepperHeight = (xFont->GetFontHeader().height + xFont->GetFontHeader().ascent/2) & 0xFFFE;
		}
		// Note that the width needs to be set before calling SetXStepper().
		mX = inXStepper->X() - (stepperHeight/5) - mWidth;
		mY = inXStepper->Y() + (stepperHeight - (int16_t)(xFont->GetFontHeader().ascent))/2;
		UpdateStringForValue();
	}
	return(xFont);
}

/********************************** SetValue **********************************/
bool XValueField::SetValue(
	int32_t	inValue,
	bool	inUpdate)
{
	bool	valueChanged = inValue != mValue && ValueIsValid(inValue);
	if (valueChanged)
	{
		mValue = inValue;
		ValueChanged(inUpdate);
	}
	return(valueChanged);
}

/******************************** ValueChanged ********************************/
/*
*	Called when the displayed value via ValueFormatter has changed or the value
*	itself changed. (e.g. the displayed value unit has changed, etc.)
*/
void XValueField::ValueChanged(
	bool	inUpdate)
{
	UpdateStringForValue();
	
	if (inUpdate)
	{
		DrawSelf();
	}
}

/****************************** MakeFontCurrent *******************************/
XFont* XValueField::MakeFontCurrent(void)
{
	XFont* xFont = nullptr;
	if (mFont)
	{
		xFont = mFont->MakeCurrent();
	}
	return(xFont);
}

