/*
*	XAnimatedFontIcon.cpp, Copyright Jonathan Mackey 2023
*	Class that cycles through a font's glyphs, treating them like icons.
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

#include "XAnimatedFontIcon.h"
#include "DisplayController.h"

/****************************** XAnimatedFontIcon *****************************/
XAnimatedFontIcon::XAnimatedFontIcon(
	int16_t			inX,
	int16_t			inY,
	uint16_t		inWidth,
	uint16_t		inHeight,
	uint16_t		inTag,
	XView*			inNextView,
	uint16_t		inFromChar,
	uint16_t		inToChar,
	XFont::Font*	inFont,
	XView*			inSuperView,
	uint16_t		inRunningColor,
	uint16_t		inStoppedColor,
	uint16_t		inBGColor)
	: XView(inX, inY, inWidth, inHeight, inTag, inNextView, nullptr,
		inSuperView, true, true), mCurrentChar(inFromChar),
	  mFromChar(inFromChar), mToChar(inToChar), mFont(inFont),
	  mRunningColor(inRunningColor), mStoppedColor(inStoppedColor),
	  mBGColor(inBGColor)
{
}

/***************************** SetAnimationPeriod *****************************/
void XAnimatedFontIcon::SetAnimationPeriod(
	uint32_t	inPeriod)
{
	mAnimationPeriod.Set(inPeriod);
	if (inPeriod)
	{
		mAnimationPeriod.Start();
	}
	mCurrentChar = mFromChar;
	DrawSelf();
}

/****************************** MakeFontCurrent *******************************/
XFont* XAnimatedFontIcon::MakeFontCurrent(void)
{
	XFont* xFont = nullptr;
	if (mFont)
	{
		xFont = mFont->MakeCurrent();
	}
	return(xFont);
}

/*********************************** Update ***********************************/
void XAnimatedFontIcon::Update(void)
{
	if (mAnimationPeriod.Passed())
	{
		mCurrentChar++;
		if (mCurrentChar > mToChar)
		{
			mCurrentChar = mFromChar;
		}
		mAnimationPeriod.Start();
		DrawSelf();
	}
}

/********************************** DrawSelf **********************************/
void XAnimatedFontIcon::DrawSelf(void)
{
	if (mFont)
	{
		XFont* xFont = MakeFontCurrent();
		xFont->SetTextColor(mAnimationPeriod.Get() ? mRunningColor : mStoppedColor);
		xFont->SetBGTextColor(mBGColor);
		
		int16_t	x = 0;
		int16_t	y = 0;
		LocalToGlobal(x, y);
		xFont->GetDisplay()->MoveTo(y, x);
		xFont->DrawCharcode(mCurrentChar);
	}
}
