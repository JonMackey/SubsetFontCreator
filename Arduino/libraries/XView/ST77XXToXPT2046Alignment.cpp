/*
*	ST77XXToXPT2046Alignment.cpp, Copyright Jonathan Mackey 2023
*	Class to align the display with the touch screen.
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

#include "ST77XXToXPT2046Alignment.h"
#include "DisplayController.h"
#ifndef __MACH__
#include "XPT2046.h"
#endif
#include "XRootView.h"

const uint16_t	ST77XXToXPT2046Alignment::kInset = 20;

/************************** ST77XXToXPT2046Alignment **************************/
ST77XXToXPT2046Alignment::ST77XXToXPT2046Alignment(
	uint16_t		inTag,
	XFont::Font*	inFont,
	XView*			inNextView,
	const char*		inInstructions)
	: XView(0, 0, 0, 0, inTag, inNextView, nullptr, nullptr, false),
	  mFont(inFont),  mInstructions(inInstructions), mDebouncePeriod(500)
{
}

/************************************ Start ***********************************/
void ST77XXToXPT2046Alignment::Start(
	XPT2046*	inTouchScreen)
{
#ifndef __MACH__
	mTouchScreen = inTouchScreen;
	mTouchScreen->GetMinMax(mSavedMinMax);
	mTouchScreen->StartAlign();
#endif
	mStep = eStart;
	mCurrentX = kInset;
	mCurrentY = kInset;
	mPrevX = mPrevAdjX = 1;
	mPrevY = mPrevAdjY = 1;
	mOKToSave = false;
	
	mSavedModalView = XRootView::GetInstance()->ModalView();
	XRootView::GetInstance()->SetModalView(this);
	XFont*	xFont = MakeFontCurrent();
	if (xFont)
	{
		DisplayController*	display = xFont->GetDisplay();
		mWidth = display->GetColumns();
		mHeight = display->GetRows();
	}
	Show();
	mDebouncePeriod.Start();
}

/************************************ Stop ************************************/
void ST77XXToXPT2046Alignment::Stop(
	bool	inRestoreMinMax)
{
	XRootView::GetInstance()->SetModalView(mSavedModalView);
	mSavedModalView = nullptr;
#ifndef __MACH__
	if (inRestoreMinMax)
	{
		mTouchScreen->SetMinMax(mSavedMinMax);
	}
#endif
	Hide();
}

/****************************** MakeFontCurrent *******************************/
XFont* ST77XXToXPT2046Alignment::MakeFontCurrent(void)
{
	XFont* xFont = nullptr;
	if (mFont)
	{
		xFont = mFont->MakeCurrent();
	}
	return(xFont);
}

/********************************** DrawSelf **********************************/
void ST77XXToXPT2046Alignment::DrawSelf(void)
{
	XFont*	xFont = MakeFontCurrent();
	if (xFont)
	{
		DisplayController*	display = xFont->GetDisplay();
		if (mStep == eStart)
		{
			uint16_t	height;
			uint16_t	width;
			xFont->MeasureStr(mInstructions, height, width);
			display->SetBGColor(XFont::eBlack);
			xFont->SetTextColor(XFont::eWhite);
			xFont->SetBGTextColor(XFont::eBlack);
			display->Fill();	// Erase display
			// Draw the mInstructions centered on the display
			display->MoveTo((display->GetRows() - height)/2,
							(display->GetColumns() - width)/2);
			xFont->DrawStr(mInstructions);
			// Draw a 30 pixel wide solid red circle at the top center of
			// the display.
			display->SetFGColor(XFont::eRed);
			display->DrawCircle(display->GetColumns()/2, 20, 15, 0);
			mStep++;
		} 
		
		if (!mOKToSave)
		{
			/*
			*	If enough points have been entered to calculate the touch screen
			*	alignment THEN
			*	Change the red circle drawn above to green.
			*
			*	Note that a green circle does NOT mean that the current
			*	caclulated alignment is any good.  The alignment can only be
			*	determined by visually checking the accuracy of the magenta dots
			*	drawn after each touch.  Keep touching the white dots till the
			*	alignment seems to be consistently accurate, and even then you
			*	may have to do the alignment over.
			*/
#ifndef __MACH__
			if (mTouchScreen->AlignmentReady())
#else
			if (mStep > 2)
#endif
			{
				mOKToSave = true;
				display->SetFGColor(XFont::eGreen);
				display->DrawCircle(display->GetColumns()/2, 20, 15, 0);
			}
		}
		
		{
			uint16_t	rows = display->GetRows();
			uint16_t	columns = display->GetColumns();
			uint16_t	newX;
			uint16_t	newY;
			switch (mStep)
			{
				case eStepTL:
					newY = kInset;
					newX = kInset;
					break;
				case eStepBR:
					newY = rows-kInset;
					newX = columns-kInset;
					break;
				case eStepTR:
					newY = kInset;
					newX = columns-kInset;
					break;
				case eStepBL:
					newY = rows-kInset;
					newX = kInset;
					break;
			}
			// Erase the location where the new circle and white dot will be drawn
			display->FillRect(newX-10, newY-10, kInset, kInset, XFont::eBlack);
			// Draw the new white circle around a white dot
			display->SetFGColor(XFont::eWhite);
			display->DrawCircle(newX, newY, 10, 2);
			// Draw the new white dot
			display->FillRect(newX-1, newY-1, 2, 2, XFont::eWhite);
			// Perform a dummy draw to flush the display command queue. 
			display->FillRect(1, 100, 2, 2, XFont::eBlack);
			mCurrentX = newX;
			mCurrentY = newY;
		}
	}
}

#ifndef __MACH__
/******************************** ToggleInvertX *******************************/
void ST77XXToXPT2046Alignment::ToggleInvertX(void)
{
	mTouchScreen->StartAlign();
	mTouchScreen->ToggleInvertX();
}

/******************************** ToggleInvertY *******************************/
void ST77XXToXPT2046Alignment::ToggleInvertY(void)
{
	mTouchScreen->StartAlign();
	mTouchScreen->ToggleInvertY();
}
#endif
/********************************* MouseDown **********************************/
void ST77XXToXPT2046Alignment::MouseDown(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	/*
	*	The goal is to not respond to every mouse down, especially when you're
	*	trying to precisely touch the tiny white dot.  Pressing slowly/precisely
	*	will generate multiple hits.  The mDebouncePeriod limits this to one hit
	*	every half second.  This should give you enough time to ease the
	*	pressure off the touch screen without having to process multiple hits.
	*/
	if (mDebouncePeriod.Passed())
	{
		mStep++;
		if (mStep == eNumSteps)
		{
			mStep = eStepTL;
		}
		uint16_t	x,y,z;
#ifndef __MACH__
		// Update the alignment by adding the current point touched.
		mTouchScreen->Align(kInset);
		// Read the touch again to display the updated alignment...
		mTouchScreen->Read(x, y, z);
#else
		x = inGlobalX;
		y = inGlobalY;
#endif
		XFont*	xFont = MakeFontCurrent();
		if (xFont)
		{
			DisplayController*	display = xFont->GetDisplay();
			// Erase the current circle and white dot
			display->SetFGColor(XFont::eBlack);
			display->DrawCircle(mCurrentX, mCurrentY, 10, 2);
			display->FillRect(mPrevX-1, mPrevY-1, 2, 2, XFont::eBlack);
			mPrevX = mCurrentX;
			mPrevY = mCurrentY;
			// Erase the previous calculated hit point (magenta dot)
			display->FillRect(mPrevAdjX-1, mPrevAdjY-1, 2, 2, XFont::eBlack);
			// If the new calculated hit point is on the display THEN
			// Draw the new calculated hit point.
			if (x > 1 && x < display->GetColumns() &&
				y > 1 && y < display->GetRows())
			{
				display->FillRect(x-1, y-1, 2, 2, XFont::eMagenta);
				mPrevAdjX = x;
				mPrevAdjY = y;
			}
			// Perform a dummy draw to flush the display command queue. 
			display->FillRect(1, 100, 2, 2, XFont::eBlack);
		}
		DrawSelf();
		mDebouncePeriod.Start();
	}
}
