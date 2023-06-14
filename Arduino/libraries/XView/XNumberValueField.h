/*
*	XNumberValueField.h, Copyright Jonathan Mackey 2023
*	The number value field associated with a stepper.
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
#ifndef XNumberValueField_h
#define XNumberValueField_h

#include "XValueField.h"
#include "ValueFormatter.h"


class XNumberValueField : public XValueField
{
public:
							XNumberValueField(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								XFont::Font*			inFont = nullptr,
								int32_t					inValue = 0,
								int32_t					inMaxValue = 100,
								int32_t					inMinValue = 0,
								uint32_t				inIncrement = 1,
								bool					inValueWraps = true,
								bool					inRoundToIncrement = false,
								ValueFormatterPtr		inValueFormatter = nullptr,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack,
								XFont::ETextAlignment	inTextAlignment = XFont::eAlignRight);
	virtual void			DrawSelf(void);
	void					SetMinMax(
								int32_t					inMinValue,
								int32_t					inMaxValue)
								{mMaximum = inMaxValue; mMinimum = inMinValue;}
	void					SetIncrement(
								int32_t					inIncrement)
								{mIncrement = inIncrement;}
	void					SetValueWraps(
								bool					inValueWraps)
								{mValueWraps = inValueWraps;}
	void					SetValueFormatter(
								ValueFormatterPtr		inValueFormatter)
								{mValueFormatter = inValueFormatter;}
	void					OverrideValueString(
								const char*				inString,
								bool					inUpdate = true);
	void					SetRoundToIncrement(
								bool					inRoundToIncrement)
								{mRoundToIncrement = inRoundToIncrement;}
	virtual bool			IncrementValue(void);
	virtual bool			DecrementValue(void);
protected:
	char				mValueString[15];
	ValueFormatterPtr	mValueFormatter;
	uint32_t			mIncrement;
	int32_t				mMinimum;
	int32_t				mMaximum;
	bool				mValueWraps;
	bool				mRoundToIncrement;

	virtual bool			ValueIsValid(
								int32_t					inValue);
	virtual void			UpdateStringForValue(void);
};
#endif // XNumberValueField_h
