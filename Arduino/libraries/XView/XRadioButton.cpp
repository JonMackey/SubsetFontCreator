/*
*	XRadioButton.cpp, Copyright Jonathan Mackey 2023
*	A radio button.
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

#include "XRadioButton.h"
#include "DisplayController.h"

/*
*	I couldn't come up with a satisfactory way of scaling the radio based on
*	the font size.  The array below is simply by trial.
*/
static const struct SRadioDim
{
	int16_t	size;	// Twice the radius
	int16_t	gap;
	int16_t	radius;
	int16_t	centerRadius;
	int16_t	thickness;
} kRadioDim[] = {
	{16, 6, 8, 4, 3},
	{14, 6, 7, 3, 3},
	{12, 4, 6, 3, 3},
	{10, 4, 5, 2, 2}
};


/******************************** XRadioButton ********************************/
/*
*	In order to initialize the root radio pointer, the last instance created of
*	the radio group should have inIsRootRadio set to true.
*
*	e.g. The order of instantiation, the last has inIsRootRadio set to true.
*		XRadioButton radioA(,,nullptr, false,,)
*		XRadioButton radioB(,,&radioA, false,,)
*		XRadioButton radioC(,,&radioB, false,,)
*		XRadioButton radioD(,,&radioC, true,,)
*/
XRadioButton::XRadioButton(
	int16_t							inX,
	int16_t							inY,
	uint16_t						inWidth,
	uint16_t						inHeight,
	uint16_t						inTag,
	XView*							inNextView,
	const char*						inString,
	XFont::Font*					inFont,
	XRadioButton*					inNextRadio,
	bool							inIsRootRadio,
	XRadioButton::ERadioSize		inRadioSize,
	uint16_t						inFGColor,
	uint16_t						inBGColor)
	: XButton(inX, inY, inWidth, inHeight, inTag, inNextView, inString, inFont,
	  inFGColor, inBGColor), mRadioSize(inRadioSize),
	  mRootRadio(nullptr), mNextRadio(inNextRadio)
{
	/*
	*	Make room for the radio by shifting the label right.
	*/
	const SRadioDim& radioDim = kRadioDim[inRadioSize];
	mLabel.MoveBy(radioDim.size+radioDim.gap, 0);
	mLabel.AdjustSize(-radioDim.size-radioDim.gap, 0);
	mLabel.SetTextColor(inFGColor);
	mLabel.SetBGColor(inBGColor);
	
	if (inIsRootRadio)
	{
		mRootRadio = this;
		for (XRadioButton* thisRadio = mNextRadio; thisRadio;
				thisRadio = thisRadio->mNextRadio)
		{
			thisRadio->SetRootRadio(this);
		}
	}
}
/********************************** DrawSelf **********************************/
void XRadioButton::DrawSelf(void)
{
	if (mVisible)
	{
		XFont*	xFont = mLabel.MakeFontCurrent();
		if (xFont)
		{
			DisplayController*	display = xFont->GetDisplay();
			uint16_t	fieldColor, radioColor;
			int16_t	x = 0;
			int16_t	y = 0;
			LocalToGlobal(x, y);
			const SRadioDim& radioDim = kRadioDim[mRadioSize];
			if (mEnabled)
			{
				fieldColor = mState ? 0xFC01 : 0xFFFF;
				radioColor = 0xFFFF;
			} else
			{
				fieldColor = 0xFFDF;
				radioColor = 0xBDF7;
			}
			display->SetFGColor(0xB5B6);
			display->SetBGColor(mBGColor);
			int16_t	adjY = (int16_t)mHeight/2;
			adjY+=y;
			display->DrawCircle(x+radioDim.radius, adjY, radioDim.radius+1, 1);
			display->SetFGColor(fieldColor);
			display->DrawCircle(x+radioDim.radius, adjY, radioDim.radius, 0);
			if (mState)
			{
				display->SetFGColor(radioColor);
				display->SetBGColor(fieldColor);
				if (mState == eOn)
				{
					display->DrawCircle(x+radioDim.radius, adjY, radioDim.centerRadius, 0);
				} else
				{
					display->DrawLine(x+radioDim.thickness, adjY,
										x+radioDim.size-radioDim.thickness,
											adjY, radioDim.thickness);
				}
			}
			mLabel.DrawSelf();
		}
	}
}

/********************************** SetState **********************************/
void XRadioButton::SetState(
	XControlState	inState,
	bool			inUpdate)
{
	if (mState != inState)
	{
		XControl::SetState(inState, false);
		if (mState == eOn)
		{
			if (mRootRadio)
			{
				// Turn off the other radio that's currently on
				for (XRadioButton* thisRadio = mRootRadio; thisRadio;
						thisRadio = thisRadio->mNextRadio)
				{
					if (thisRadio != this &&
						thisRadio->GetState() == eOn)
					{
						thisRadio->SetState(eOff);
					}
				}
			}
		}
		if (inUpdate)DrawSelf();
	}
}

/********************************* MouseDown **********************************/
void XRadioButton::MouseDown(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	if (mState == eOff)
	{
		SetState(eOn);
	}
}
