/*
*	XDateValueField.h, Copyright Jonathan Mackey 2023
*	The date value field that can be associated with a stepper.
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
#ifndef XDateValueField_h
#define XDateValueField_h

#include "XValueField.h"
#include "UnixTime.h"
class XDateValueField : public XValueField
{
public:
							XDateValueField(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								XFont::Font*			inFont = nullptr,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack);
	virtual void			DrawSelf(void);
	virtual void			MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	virtual bool			IncrementValue(void);
	virtual bool			DecrementValue(void);
	virtual void			ValueChanged(
								bool					inUpdate = true);
protected:
		// 04-MAR-2023, 10:11:20 PM

	struct SDateStrings
	{
		char	second[3];
		char	minute[3];
		char	hour[3];
		char	day[3];
		char	month[4];
		char	year[5];
	};
	enum EDateSubField
	{
		eAMPMField,
		eSecondField,
		eMinuteField,
		eHourField,
		eDayField,
		eMonthField,
		eYearField,
		eHourSepField,
		eMinuteSepField,
		eSecondSepField,
		eDaySepField,
		eMonthSepField,
		eYearSepField,
		eNumSubFields,
		eNoSubField
	};
	int32_t			mLastValue;
	uint8_t			mActiveField;
	uint16_t		mDirtyField;
	static const uint8_t	kFieldOrder[eNumSubFields];
	uint8_t			mFieldX[eNumSubFields];
	uint8_t			mFieldWidth[eNumSubFields];
	UnixTime::SComponents	mComponents;
	const char*		mSubFieldStrs[eNumSubFields];
	SDateStrings	mDateStrings;

	virtual bool			ValueIsValid(
								int32_t					inValue);
	virtual void			UpdateStringForValue(void);
	void					UpdateStringsFromComponents(void);
	void					DrawFields(void);
	bool					IncDecValue(
								bool					inIncrement);
};
#endif // XDateValueField_h
