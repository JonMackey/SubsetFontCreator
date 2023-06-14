/*
*	XLabel.cpp, Copyright Jonathan Mackey 2023
*	Base class for a text label.
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

#include "XLabel.h"
#include "DisplayController.h"

/*********************************** XLabel ***********************************/
XLabel::XLabel(
	int16_t					inX,
	int16_t					inY,
	uint16_t				inWidth,
	uint16_t				inHeight,
	uint16_t				inTag,
	XView*					inNextView,
	const char*				inString,
	XFont::Font*			inFont,
	XView*					inSuperView,
	uint16_t				inTextColor,
	uint16_t				inBGColor,
	XFont::ETextAlignment	inTextAlignment,
	bool					inEnabled)
	: XView(inX, inY, inWidth, inHeight, inTag, inNextView, nullptr,
		inSuperView, true, inEnabled),  mString(inString), mFont(inFont),
	  mBGColor(inBGColor), mTextColor(inTextColor),
	  mTextAlignment(inTextAlignment), mEraseUnusedAreaAfterDraw(false)
{
}

/****************************** MakeFontCurrent *******************************/
XFont* XLabel::MakeFontCurrent(void)
{
	XFont* xFont = nullptr;
	if (mFont)
	{
		xFont = mFont->MakeCurrent();
	}
	return(xFont);
}

/********************************** DrawSelf **********************************/
void XLabel::DrawSelf(void)
{
	if (mString && mFont)
	{
		XFont*	xFont = MakeFontCurrent();
		uint16_t	textColor = mEnabled ? mTextColor :
						DisplayController::Calc565Color(mTextColor, 0, 184);
		xFont->SetTextColor(textColor);
		xFont->SetBGTextColor(mBGColor);
		int16_t	x = 0;
		int16_t	y = 0;
		LocalToGlobal(x, y);
		if (mTextAlignment != XFont::eAlignLeft ||
			mEraseUnusedAreaAfterDraw)
		{
			xFont->DrawAligned(mString, x, y, mWidth, mTextAlignment, mEraseUnusedAreaAfterDraw);
		} else
		{
			xFont->GetDisplay()->MoveTo(y, x);
			xFont->DrawStr(mString);
		}
	}
}
