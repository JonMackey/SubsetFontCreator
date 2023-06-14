/*
*	XValueField.h, Copyright Jonathan Mackey 2023
*	Base class for a stepper value field.
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
#ifndef XValueField_h
#define XValueField_h

#include "XControl.h"
#include "XFont.h"

class XStepper;

class XValueField : public XControl
{
public:
							XValueField(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								XFont::Font*			inFont = nullptr,
								int32_t					inValue = 0,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack,
								XFont::ETextAlignment	inTextAlignment = XFont::eAlignRight);
	XFont*					SetXStepper(
								XStepper*				inXStepper);
	int32_t					Value(void) const
								{return(mValue);}
	virtual bool			SetValue(
								int32_t					inValue,
								bool					inUpdate=true);
	XFont*					MakeFontCurrent(void);
	inline XFont::Font*		GetFont(void)
								{return(mFont);}
	void					SetFont(
								XFont::Font*			inFont)
								{mFont = inFont;}
	inline uint16_t			GetFGColor(void) const
								{return(mFGColor);}
	void					SetFGColor(
								uint16_t				inFGColor)
								{mFGColor = inFGColor;}
	inline uint16_t			GetBGColor(void) const
								{return(mBGColor);}
	void					SetBGColor(
								uint16_t				inBGColor)
								{mBGColor = inBGColor;}
	XFont::ETextAlignment	GetTextAlignment(void) const
								{return(mTextAlignment);}
	void					SetTextAlignment(
								XFont::ETextAlignment	inTextAlignment)
								{mTextAlignment = inTextAlignment;}
	virtual bool			IncrementValue(void) = 0;
	virtual bool			DecrementValue(void) = 0;
	virtual void			ValueChanged(
								bool					inUpdate = true);
protected:
	int32_t					mValue;
	XStepper*				mStepper;			
	XFont::Font*			mFont;
	uint16_t				mFGColor;
	uint16_t				mBGColor;
	XFont::ETextAlignment	mTextAlignment;

	virtual bool			ValueIsValid(
								int32_t					inValue) = 0;
	virtual void			UpdateStringForValue(void) = 0;
};
#endif // XValueField_h
