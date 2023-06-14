/*
*	XCheckboxButton.cpp, Copyright Jonathan Mackey 2023
*	A checkbox button.
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

#include "XCheckboxButton.h"
#include "DisplayController.h"

/*
*	I couldn't come up with a satisfactory way of scaling the checkbox based on
*	the font size.  The array below is simply by trial.
*/
static const struct SCheckDim
{
	int16_t	size;
	int16_t	gap;
	int16_t	radius;
	int16_t	thickness;
	int16_t	check[8];
} kCheckDim[] = {
	{18, 6, 5, 3, {4,9,7,13,13,5,6,12}},
	{16, 6, 5, 3, {4,9,6,11,11,5,5,10}},
	{14, 4, 3, 3, {4,7,6,9,10,4,5,8}},
	{10, 4, 1, 2, {2,4,4,6,7,3,3,5}}
};

/******************************** XCheckboxButton *********************************/
XCheckboxButton::XCheckboxButton(
	int16_t							inX,
	int16_t							inY,
	uint16_t						inWidth,
	uint16_t						inHeight,
	uint16_t						inTag,
	XView*							inNextView,
	const char*						inString,
	XFont::Font*					inFont,
	XCheckboxButton::ECheckSize		inCheckSize,
	uint16_t						inFGColor,
	uint16_t						inBGColor)
	: XButton(inX, inY, inWidth, inHeight, inTag, inNextView, inString, inFont,
	  inFGColor, inBGColor), mCheckSize(inCheckSize)
{
	/*
	*	Make room for the checkbox by shifting the label right.
	*/
	const SCheckDim& checkDim = kCheckDim[inCheckSize];
	mLabel.MoveBy(checkDim.size+checkDim.gap, 0);
	mLabel.AdjustSize(-checkDim.size-checkDim.gap, 0);
	mLabel.SetTextColor(inFGColor);
	mLabel.SetBGColor(inBGColor);
}
/********************************** DrawSelf **********************************/
void XCheckboxButton::DrawSelf(void)
{
	if (mVisible)
	{
		XFont*	xFont = mLabel.MakeFontCurrent();
		if (xFont)
		{
			DisplayController*	display = xFont->GetDisplay();
			uint16_t	fieldColor, checkColor;
			int16_t	x = 0;
			int16_t	y = 0;
			LocalToGlobal(x, y);
			const SCheckDim& checkDim = kCheckDim[mCheckSize];
			if (mEnabled)
			{
				fieldColor = mState ? 0xFC01 : 0xFFFF;
				checkColor = 0xFFFF;
			} else
			{
				fieldColor = 0xFFDF;
				checkColor = 0xBDF7;
			}
			display->SetFGColor(0xB5B6);
			display->SetBGColor(mBGColor);
			int16_t	adjY = ((int16_t)mHeight - checkDim.size)/2;
			if (adjY < 0)
			{
				adjY = 0;
			}
			adjY+=y;
			display->DrawRoundedRect(x,adjY,checkDim.size,checkDim.size,checkDim.radius,255, true);
			display->SetFGColor(fieldColor);
			display->DrawRoundedRect(x+1,adjY+1,checkDim.size-2,checkDim.size-2,checkDim.radius,255);
			if (mState)
			{
				display->SetFGColor(checkColor);
				display->SetBGColor(fieldColor);
				if (mState == eOn)
				{
					display->DrawLine(x+checkDim.check[0], adjY+checkDim.check[1],
										x+checkDim.check[2], adjY+checkDim.check[3],
											checkDim.thickness);
					display->DrawLine(x+checkDim.check[2], adjY+checkDim.check[3],
										x+checkDim.check[4], adjY+checkDim.check[5],
											checkDim.thickness);
					display->FillRect(x+checkDim.check[6], adjY+checkDim.check[7], 1, 1, checkColor);
				} else
				{
					int16_t	halfY = adjY + (checkDim.size/2);
					display->DrawLine(x+checkDim.check[0], halfY,
										x+checkDim.check[4], halfY,
											checkDim.thickness);
				}
			}
			mLabel.DrawSelf();
		}
	}
}

/********************************* MouseDown **********************************/
void XCheckboxButton::MouseDown(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	if (mAllowsTristate)
	{
		int8_t	newState = mState+1;
		SetState(newState > eMixed ? eOff : (XControlState)newState);
	} else if (mState != eMixed)
	{
		SetState(mState == eOn ? eOff : eOn);
	}
}
