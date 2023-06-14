/*
*	XStepper.cpp, Copyright Jonathan Mackey 2023
*	A value stepper.
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

#include "XStepper.h"
#include "XValueField.h"
#include "DisplayController.h"
#ifdef __MACH__
#include <stdio.h>
#endif

/********************************** XStepper **********************************/
XStepper::XStepper(
	int16_t				inX,
	int16_t				inY,
	uint16_t			inWidth,
	uint16_t			inHeight,
	uint16_t			inTag,
	XValueField*		inValueField)
	: XControl(inX, inY, inWidth, inHeight, inTag, inValueField),
	  mValueField(inValueField)
{
	if (inValueField)
	{
		XFont*	xFont = inValueField->SetXStepper(this);
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
				mHeight = (xFont->GetFontHeader().height + xFont->GetFontHeader().ascent/2) & 0xFFFE;
			}
			mWidth = (mHeight*10)/15;
			if ((mWidth & 1) == 0)
			{
				mWidth++;
			}
			mRadius = mHeight/5;
		}
	}
/*
autorepeat (not implemented yet)
True if the first mouse down does one increment (or decrement) and, after a
delay of 0.5 seconds, increments (or decrements) at a rate of ten times per
second. False if the receiver does one increment (decrement) on a mouse up. The
default is true.


*/
}

/********************************** DrawSelf **********************************/
void XStepper::DrawSelf(void)
{
	if (mVisible)
	{
		DrawStepper(eUpAndDownParts);
	}
}

/******************************** DrawStepper *********************************/
void XStepper::DrawStepper(
	uint8_t	inPart)
{
	if (inPart != eNoButtonPart &&
		mValueField)
	{
		XFont*	xFont = mValueField->MakeFontCurrent();
		if (xFont)
		{
			DisplayController*	display = xFont->GetDisplay();
			int16_t	x = 0;
			int16_t	y = 0;
			LocalToGlobal(x, y);
			int16_t	radiusX2 = mRadius*2;
			int16_t widthMRX2 = mWidth-radiusX2;
			int16_t	halfWidth = mWidth/2;
			int16_t heightMRX2 = mHeight-radiusX2;
			int16_t	halfHeight = mHeight/2;
			int16_t	halfHeightMR = halfHeight - mRadius;
			int16_t xPlusR = x+mRadius;
			int16_t yPlusR = y+mRadius;
			uint16_t	fillColor, frameColor;
			uint8_t	frameTint;
			uint16_t	bgColor = mValueField->GetBGColor();
			uint16_t	fgColor = XFont::eWhite;
			if (inPart & eUpButtonPart)
			{
				display->SetFGColor(0xB5B6);
				display->SetBGColor(bgColor);
				frameTint = display->DrawCircle(xPlusR, yPlusR, mRadius, 1, DisplayController::eNorthHalf, widthMRX2, heightMRX2);
				frameColor = display->Calc565Color(frameTint);
				display->FillRect(xPlusR,y, widthMRX2, 1, frameColor);	// Top
				display->FillRect(x, yPlusR, 1, halfHeightMR, frameColor);	// Left
				display->FillRect(x+mWidth-1, yPlusR, 1, halfHeightMR, frameColor);// Right
				display->FillRect(x+1, y+halfHeight-1, mWidth-2, 1, 0xE71C);	// Separator

				fillColor = (mState & eOn) ? 0xEF5D : fgColor;
				//fillColor = DisplayController::Calc565Color(fgColor, bgColor, (mState & eOn) ? 170 : 255);
				display->SetFGColor(fillColor);

				display->DrawCircle(xPlusR, yPlusR, mRadius-1, 0, DisplayController::eNorthHalf, widthMRX2, heightMRX2);
				display->FillRect(x+1, yPlusR, mRadius-1, halfHeightMR-1, fillColor);
				display->FillRect(x+mWidth-mRadius, yPlusR, mRadius-1, halfHeightMR-1, fillColor);
				display->FillRect(xPlusR, y+1, widthMRX2, halfHeight-2, fillColor);
		
				display->SetFGColor(XFont::eBlack);
				display->SetBGColor(fgColor);
				display->DrawLine(x+mRadius, y+halfWidth-1, x+halfWidth, y+mRadius-1, 2);
				display->DrawLine(x+mWidth-mRadius-1, y+halfWidth-1, x+halfWidth, y+mRadius-1, 2);
			}
			if (inPart & eDownButtonPart)
			{
				display->SetFGColor(0xB5B6);
				display->SetBGColor(bgColor);
				frameTint = display->DrawCircle(xPlusR, yPlusR, mRadius, 1, DisplayController::eSouthHalf, widthMRX2, heightMRX2);
				frameColor = display->Calc565Color(frameTint);
				display->FillRect(x+1, y+halfHeight, mWidth-2, 1, 0xE71C);	// Separator
				display->FillRect(x, y+halfHeight, 1, halfHeightMR, frameColor);	// Left
				display->FillRect(x+mWidth-1, y+halfHeight, 1, halfHeightMR, frameColor);// Right
				display->FillRect(xPlusR,y+mHeight-1, widthMRX2, 1, frameColor);	// Bottom

				fillColor = (mState & eDownOn) ? 0xEF5D : fgColor;
				//fillColor = DisplayController::Calc565Color(fgColor, bgColor, (mState & eDownOn) ? 170 : 255);
				display->SetFGColor(fillColor);

				display->DrawCircle(xPlusR, yPlusR, mRadius-1, 0, DisplayController::eSouthHalf, widthMRX2, heightMRX2);
				display->FillRect(x+1, y+halfHeight+1, mRadius-1, halfHeightMR, fillColor);
				display->FillRect(x+mWidth-mRadius, y+halfHeight+1, mRadius-1, halfHeightMR, fillColor);
				display->FillRect(xPlusR, y+halfHeight+1, widthMRX2, halfHeight-2, fillColor);
		
				display->SetFGColor(XFont::eBlack);
				display->SetBGColor(fgColor);
				display->DrawLine(x+mRadius, y+mHeight-halfWidth, x+halfWidth, y+mHeight-mRadius, 2);
				display->DrawLine(x+mWidth-mRadius-1, y+mHeight-halfWidth, x+halfWidth, y+mHeight-mRadius, 2);
			}
		}
	}
}

/********************************* GetPartHit *********************************/
XStepper::XStepperPart XStepper::GetPartHit(
	int16_t	inGlobalX,
	int16_t	inGlobalY) const
{
	XStepper::XStepperPart partHit = eNoButtonPart;
	int16_t	x = 0;
	int16_t	y = 0;
	
	LocalToGlobal(x, y);
	if (inGlobalX >= x &&
		inGlobalY >= y &&
		inGlobalX < (x+mWidth))
	{
		if (inGlobalY < (y+(mHeight/2)))
		{
			partHit = eUpButtonPart;
		} else if (inGlobalY < (y+mHeight))
		{
			partHit = eDownButtonPart;
		}
	}
	return(partHit);
}

/********************************* MouseDown **********************************/
void XStepper::MouseDown(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	if (mValueField)
	{
		XStepperPart partHit = GetPartHit(inGlobalX, inGlobalY);
		uint16_t	state = mState;
		XControlState	newState = mState;
		if (partHit == eUpButtonPart)
		{
			state |= eOn;
			newState = eOn;
			
			mValueField->IncrementValue();
		} else if (partHit == eDownButtonPart)
		{
			state |= eDownOn;
			newState = eDownOn;
			
			mValueField->DecrementValue();
		} else
		{
			newState = eOff;
		}
		if (state != mState)
		{
			XControl::SetState(newState, false);
			DrawStepper(state != mState ? eUpAndDownParts : partHit);
		}
	}
}

/********************************** MouseUp ***********************************/
void XStepper::MouseUp(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	uint8_t	partToDraw = eNoButtonPart;
	if (mState & eOn)
	{
		partToDraw = eUpButtonPart;
	}
	if (mState & eDownOn)
	{
		partToDraw += eDownButtonPart;
	}
	XControl::SetState(eOff, false);
	if (partToDraw)
	{
		DrawStepper(partToDraw);
	}
}

