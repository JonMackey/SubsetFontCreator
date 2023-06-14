/*
*	XButton.cpp, Copyright Jonathan Mackey 2023
*	Base class for a button.
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

#include "XButton.h"


/********************************** XButton ***********************************/
XButton::XButton(
	int16_t					inX,
	int16_t					inY,
	uint16_t				inWidth,
	uint16_t				inHeight,
	uint16_t				inTag,
	XView*					inNextView,
	const char*				inString,
	XFont::Font*			inFont,
	uint16_t				inFGColor,
	uint16_t				inBGColor,
	XFont::ETextAlignment	inTextAlignment)
	: XControl(inX, inY, inWidth, inHeight, inTag, inNextView),
	  mLabel(0, 0, inWidth, inHeight, 0, nullptr, inString, inFont, this,
	  inBGColor, inFGColor, inTextAlignment),
	  mFGColor(inFGColor), mBGColor(inBGColor)
{
	XFont*	xFont = mLabel.MakeFontCurrent();
	if (xFont)
	{
		/*
		*	If the height is zero THEN
		*	calculate the height based on the font.
		*	Note that this only works on fonts created with the minimized height
		*	set.  If the font has too few glyphs, the result is unpredictable.
		*/
		if (inHeight == 0)
		{
			//mHeight = xFont->GetFontHeader().ascent*2;
			mHeight = xFont->GetFontHeader().height + xFont->GetFontHeader().ascent/2;
			mLabel.SetHeight(xFont->GetFontHeader().height);
		}
		mLabel.SetOrigin(0, ((int16_t)mHeight - (int16_t)(xFont->GetFontHeader().ascent))/2);
		
		/*
		*	If the width is zero AND
		*	there is a string THEN
		*	calculate the button width as the text width plus padding
		*/
		if (inWidth == 0 &&
			inString)
		{
			uint16_t	height;
			uint16_t	width;
			xFont->MeasureStr(inString, height, width);
			width += 20;
			mWidth = width;
			mLabel.SetWidth(width);
		}
	}
}

/********************************** SetString *********************************/
/*
*	If you're going to set the button string, make sure you assign the widest
*	string in the constructor (if using the constructor to set the width of the
*	button.)
*/
void XButton::SetString(
	const char*	inString,
	bool		inUpdate)
{
	if (inString != mLabel.GetString())
	{
		mLabel.SetString(inString);
		if (inUpdate)
		{
			DrawSelf();
		}
	}
}
								
/********************************** SetSize ***********************************/
void XButton::SetSize(
	uint16_t	inWidth,
	uint16_t	inHeight)
{
	int16_t widthDelta = (int16_t)inWidth-mWidth;
	mWidth = inWidth;
	mHeight = inHeight;
	mLabel.AdjustSize(widthDelta, 0);
}

/********************************** SetWidth **********************************/
void XButton::SetWidth(
	uint16_t	inWidth)
{
	int16_t widthDelta = (int16_t)inWidth-mWidth;
	mWidth = inWidth;
	mLabel.AdjustSize(widthDelta, 0);
}

/********************************* AdjustSize *********************************/
void XButton::AdjustSize(
	int16_t	inWidthAdj,
	int16_t	inHeightAdj)
{
	mWidth += inWidthAdj;
	mHeight += inHeightAdj;
	mLabel.AdjustSize(inWidthAdj, 0);
}

/*********************************** Enable ***********************************/
void XButton::Enable(
	bool	inEnabled,
	bool	inUpdate)
{
	mLabel.Enable(inEnabled, false);
	XControl::Enable(inEnabled, inUpdate);
}

/****************************** SetTextAlignment ******************************/
void XButton::SetTextAlignment(
	XFont::ETextAlignment	inTextAlignment)
{
	mLabel.SetTextAlignment(inTextAlignment);
}
