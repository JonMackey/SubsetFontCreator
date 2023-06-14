/*
*	ST77XXToXPT2046Alignment.h, Copyright Jonathan Mackey 2023
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
#ifndef ST77XXToXPT2046Alignment_h
#define ST77XXToXPT2046Alignment_h

#include "XView.h"
#include "XFont.h"
#include "MSPeriod.h"

#ifndef __MACH__
class XPT2046;
#else
#define XPT2046 uint32_t
#endif

class ST77XXToXPT2046Alignment : public XView
{
public:
							ST77XXToXPT2046Alignment(
								uint16_t				inTag,
								XFont::Font*			inFont,
								XView*					inNextView,
								const char*				inInstructions);
	XFont*					MakeFontCurrent(void);
	virtual void			DrawSelf(void);
	virtual bool			HitSelf(
								int16_t					inLocalX,
								int16_t					inLocalY)
								{return(mVisible);}
	virtual void			MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	void					Start(
								XPT2046*				inTouchScreen);
	void					Stop(
								bool					inRestoreMinMax);
#ifndef __MACH__
	void					ToggleInvertX(void);
	void					ToggleInvertY(void);
#endif
	bool					OKToSave(void) const
								{return(mOKToSave);}
	
protected:
	XFont::Font*		mFont;
#ifndef __MACH__
	XPT2046*			mTouchScreen;
#endif
	XView*				mSavedModalView;
	MSPeriod			mDebouncePeriod;
	uint16_t			mSavedMinMax[4];
	const char*			mInstructions;
	uint16_t			mStep;
	uint16_t			mCurrentX;
	uint16_t			mCurrentY;
	uint16_t			mPrevX;
	uint16_t			mPrevY;
	uint16_t			mPrevAdjX;
	uint16_t			mPrevAdjY;
	bool				mOKToSave;
	static const uint16_t	kInset;
	
	enum ESteps
	{
		eStart,
		eStepTL,
		eStepBR,
		eStepTR,
		eStepBL,
		eNumSteps
	};
};
#endif // ST77XXToXPT2046Alignment_h
