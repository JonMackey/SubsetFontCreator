/*
*	FilterStatusGauge.h, Copyright Jonathan Mackey 2023
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
#ifndef FilterStatusGauge_h
#define FilterStatusGauge_h

#include "MSPeriod.h"
#include "XFont.h"
#include "XColoredView.h"

/*
*	The gauge is drawn in the center of the display (for now anyway.)
*	The left and top are assumed to be 0 (so mX and mY are only used by the
*	base XView class.)
*	The angle of the gauge ends is 45°.
*/
class FilterStatusGauge : public XColoredView
{
public:
							FilterStatusGauge(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XView*					inNextView,
								XView*					inSubViews,
								XFont::Font*			inFont,
								uint16_t				inGaugeThickness = 35,
								uint16_t				inInfoFrameRadius = 105);
	void					Update(void);
	void					SetMinMax(
								int32_t				inMin,
								int32_t				inMax);
	void					SetValue(
								int32_t				inValue);
	virtual void			DrawSelf(void);
	virtual bool			HitSelf(
								int16_t					inLocalX,
								int16_t					inLocalY)
								{return(false);}
	enum ESetup
	{
		eBackColor		= 0,		// Black
		eIndicatorColor	= 0xFFFF,	// White
		eStartColor		= 0x4665,	// Green
		eCenterColor	= 0x07FF,	// Yellow
		eEndColor		= 0x001F	// Red
		//eStartColor	= 0x07C0,	// Green
		//eCenterColor	= 0x07DF,	// Yellow
		//eEndColor		= 0x31DF	// Red
	};
protected:
	MSPeriod			mAnimationPeriod;
	XFont::Font*		mFont;
	uint16_t			mLeft;
	uint16_t			mRadius;
	uint16_t			mGaugeThickness;
	uint16_t			mInfoFrameRadius;
	uint16_t			mInfoFrameThickness;
	uint16_t			mIndicatorThickness;
	uint16_t			mInfoStrTop;
	uint16_t			mInfoStrLeft;
	uint16_t			mDigitWidth;
	int32_t				mGaugeWidth;
	int32_t				mIndicatorPos;			// Used by Update
	int32_t				mIndicatorPercentage;	// Used by Update
	int32_t				mPos;			// Desired indicator position (mapped)
	int32_t				mMin;
	int32_t				mMax;
	static const int32_t	kIndicatorGap;	// Gap between indicator and gauge
	static const int32_t	kIndicatorThickness;
	static const int32_t	kInfoFrameThickness;

	static void				GenerateTransitionLine(
								uint16_t				inFromColor,
								uint16_t				inToColor,
								uint16_t				inNumSteps,
								uint16_t*				outLine);
	void					DrawGauge(void);
#if 0
	uint16_t				TransColorAtPos(
								uint16_t				inPosition);
	static uint16_t			CalcTransitionColor(
								uint16_t				inFromColor,
								uint16_t				inToColor,
								uint16_t				inNumSteps,
								uint16_t				inStep);
#endif
};

#endif // FilterStatusGauge_h
