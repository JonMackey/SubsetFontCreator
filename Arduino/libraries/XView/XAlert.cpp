/*
*	XAlert.cpp, Copyright Jonathan Mackey 2023
*
*	Class that draws an alert dialog box (modal)
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

#include "XAlert.h"

/********************************* XAlert *********************************/
XAlert::XAlert(
	uint16_t				inTag,
	XView*					inNextView,
	const char*				inOKButtonText,
	const char*				inCancelButtonText,
	const char*				inTitle,
	XFont::Font*			inFont,
	XViewChangedDelegate*	inViewChangedDelegate,
	uint16_t				inFGColor,
	uint16_t				inBGColor)
	: XDialogBox(nullptr, inTag, inNextView, inOKButtonText,
		inCancelButtonText, inTitle, inFont, inViewChangedDelegate,
		inFGColor, inBGColor),
	  mMessageLabel(0,0,0,0,110, nullptr, nullptr, inFont, nullptr,
	  	0, inFGColor, XFont::eAlignLeft), mMessageTag(0)
{
}

/********************************** DoMessage *********************************/
void XAlert::DoMessage(
	const char*	inMessage,
	uint16_t	inMessageTag)
{
	if (inMessage)
	{
		mMessageTag = inMessageTag;
		mAutoSizeApplied = false;
		if (mMessageLabel.SuperView() == nullptr)
		{
			mOKButton.SetNextView(&mMessageLabel);
		}
		XFont* xFont = mMessageLabel.MakeFontCurrent();
		if (xFont)
		{
			uint16_t	height;
			uint16_t	width;
			mMessageLabel.SetString(inMessage);
			xFont->MeasureStr(inMessage, height, width);
			mMessageLabel.SetSize(width, height);
			mMessageLabel.SetOrigin(0,0);
			Show();
		}
	}
}

