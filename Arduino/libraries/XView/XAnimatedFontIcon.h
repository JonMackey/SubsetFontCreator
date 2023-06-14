/*
*	XAnimatedFontIcon.h, Copyright Jonathan Mackey 2023
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
#ifndef XAnimatedFontIcon_h
#define XAnimatedFontIcon_h

#include "XView.h"
#include "XFont.h"
#include "MSPeriod.h"

class XAnimatedFontIcon : public XView
{
public:
							XAnimatedFontIcon(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								uint16_t				inFromChar = 'A',
								uint16_t				inToChar = 'B',
								XFont::Font*			inFont = nullptr,
								XView*					inSuperView = nullptr,
								uint16_t				inRunningColor = 0xFFFF,	// White
								uint16_t				inStoppedColor = 0xCE79,	// Gray
								uint16_t				inBGColor = 0);				// Black
	void					Update(void);
							// Start/Stop.  Stop = 0
	void					SetAnimationPeriod(
								uint32_t				inPeriod);
								
	virtual void			DrawSelf(void);
	XFont*					MakeFontCurrent(void);
protected:
	MSPeriod		mAnimationPeriod;
	uint16_t		mCurrentChar;
	uint16_t		mFromChar;
	uint16_t		mToChar;
	XFont::Font*	mFont;
	uint16_t		mBGColor;
	uint16_t		mRunningColor;
	uint16_t		mStoppedColor;
};

#endif // XAnimatedFontIcon_h
