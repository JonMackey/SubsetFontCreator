/*
*	XPushButton.cpp, Copyright Jonathan Mackey 2023
*	A push button.
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

#include "XPushButton.h"
#include "DisplayController.h"


/******************************** XPushButton *********************************/
XPushButton::XPushButton(
	int16_t			inX,
	int16_t			inY,
	uint16_t		inWidth,
	uint16_t		inHeight,
	uint16_t		inTag,
	XView*			inNextView,
	const char*		inString,
	XFont::Font*	inFont,
	uint16_t		inFGColor,
	uint16_t		inBGColor)
	: XButton(inX, inY, inWidth, inHeight, inTag, inNextView, inString, inFont,
	  inFGColor, inBGColor, XFont::eAlignCenter)
{
	mLabel.SetTextColor(0);
}

/********************************** DrawSelf **********************************/
void XPushButton::DrawSelf(void)
{
	if (mVisible)
	{
		XFont*	xFont = mLabel.MakeFontCurrent();
		if (xFont)
		{
			DisplayController*	display = xFont->GetDisplay();
			int16_t	x = 0;
			int16_t	y = 0;
			LocalToGlobal(x, y);
			display->SetFGColor(0xB5B6);
			display->SetBGColor(mBGColor);
			display->DrawRoundedRect(x, y, mWidth, mHeight, mHeight/4, 255, true);
			display->SetFGColor(mFGColor);
			uint16_t	fillColor = display->DrawRoundedRect(x+1, y+1, mWidth-2, mHeight-2, mHeight/4, mState ? 200 : 255);
			mLabel.SetBGColor(fillColor);
			mLabel.SetTextColor(mEnabled ? XFont::eBlack : 0xCE79);
			mLabel.DrawSelf();
		}
	}
}

/********************************* MouseDown **********************************/
void XPushButton::MouseDown(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	SetState(eOn);
}

/********************************** MouseUp ***********************************/
void XPushButton::MouseUp(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	SetState(eOff);
}
