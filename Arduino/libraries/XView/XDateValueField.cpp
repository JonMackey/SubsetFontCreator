/*
*	XDateValueField.cpp, Copyright Jonathan Mackey 2023
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

#include "XDateValueField.h"
#include "DisplayController.h"

#ifdef __MACH__
#define _BV(bit) (1 << (bit))
#endif

const char kAMStr[] = "AM";
const char kPMStr[] = "PM";
//#define USE_MONTH_NAME	1
#ifdef USE_MONTH_NAME
const char kDateSepStr[] = "-";
#else
const char kDateSepStr[] = "/";
#endif
const char kTimeSepStr[] = ":";
const char kCommaSpaceStr[] = ", ";
const uint16_t	kSelectedFieldBGColor = 0xFB00; // Dark blue

/*
*	ToDo:
*	- Support 24 hour format
*		In order to support a 24 hour format the AM/PM field needs to either be
*		disabled or hidden.  If hidden, the fields or the stepper need to be
*		moved.  It needs to be decided whether to pick up the format flag
*		directly from UnixTime::Format24Hour() or have a member var for this.
*	- Support other field orders/separators (dynamic, not static)
*	- Support UnixTime::TimeChanged() so that the time can be updated while
*		editing.  This means the edited time automatically increases for every
*		tick of the RTC. I found it's a pain to try to synchronize pressing the
*		Set button in the dialog with the edited time.
*/
// Field display order.  Defined in case in the future the date needs to be
// displayed based on locale.
const uint8_t	XDateValueField::kFieldOrder[eNumSubFields] =
					{
					#ifdef USE_MONTH_NAME
						eDayField, eDaySepField,
						eMonthField, eMonthSepField,
					#else
						eMonthField, eMonthSepField,
						eDayField, eDaySepField,
					#endif
						eYearField, eYearSepField,
						eHourField, eHourSepField,
						eMinuteField, eMinuteSepField,
						eSecondField, eSecondSepField,
						eAMPMField
					};
/****************************** XDateValueField *****************************/
XDateValueField::XDateValueField(
	int16_t			inX,		// Not used when aligned to stepper
	int16_t			inY,		// Not used when aligned to stepper
	uint16_t		inWidth,
	uint16_t		inTag,
	XView*			inNextView,
	XFont::Font*	inFont,
	uint16_t		inFGColor,
	uint16_t		inBGColor)
	: XValueField(inX, inY, inWidth, inTag, inNextView, inFont, 0, inFGColor, inBGColor),
	  mActiveField(eNoSubField), mLastValue(0)
{
	XFont*	xFont = MakeFontCurrent();
	if (xFont)
	{
		mSubFieldStrs[eAMPMField] = kAMStr;
		
		mSubFieldStrs[eSecondField] = mDateStrings.second;
		mSubFieldStrs[eMinuteField] = mDateStrings.minute;
		mSubFieldStrs[eHourField] = mDateStrings.hour;
		mSubFieldStrs[eDayField] = mDateStrings.day;
		mSubFieldStrs[eMonthField] = mDateStrings.month;
		mSubFieldStrs[eYearField] = mDateStrings.year;

		mSubFieldStrs[eHourSepField] = kTimeSepStr;
		mSubFieldStrs[eMinuteSepField] = kTimeSepStr;
		mSubFieldStrs[eSecondSepField] = &kCommaSpaceStr[1];
		mSubFieldStrs[eDaySepField] = kDateSepStr;
		mSubFieldStrs[eMonthSepField] = kDateSepStr;
		mSubFieldStrs[eYearSepField] = kCommaSpaceStr;

		/*
		*	Initialize the ranges of the date sub fields
		*/
		xFont->LoadGlyph('0');
		uint16_t	digit00Width = xFont->Glyph().advanceX;
		digit00Width *= 2;
		xFont->LoadGlyph('-');
		uint16_t	dateSepWidth = xFont->Glyph().advanceX;
		xFont->LoadGlyph(':');
		uint16_t	timeSepWidth = xFont->Glyph().advanceX;
		xFont->LoadGlyph(',');
		uint16_t	commaWidth = xFont->Glyph().advanceX;
		xFont->LoadGlyph(' ');
		uint16_t	spaceWidth = xFont->Glyph().advanceX;
		uint16_t	unusedHeight;
		uint16_t	amPmWidth;
		uint16_t	strWidth;
		xFont->MeasureStr(kAMStr, unusedHeight, amPmWidth);
		xFont->MeasureStr(kPMStr, unusedHeight, strWidth);
		if (strWidth > amPmWidth)
		{
			amPmWidth = strWidth;
		}
	#ifdef USE_MONTH_NAME
		// March (MAR) appears to be the longest 3 letter month code.
		char monthStr[4];
		uint16_t	monthWidth;
		UnixTime::CreateMonthStr(3, monthStr);
		xFont->MeasureStr(monthStr, unusedHeight, monthWidth);
		mFieldWidth[eMonthField] = monthWidth;
	#else
		mFieldWidth[eMonthField] = digit00Width;
	#endif

		// Format ->  04-MAR-2023, 10:11:20 PM		
		mFieldWidth[eDayField] = digit00Width;
		mFieldWidth[eDaySepField] = dateSepWidth;
		mFieldWidth[eMonthSepField] = dateSepWidth;
		mFieldWidth[eYearField] = digit00Width*2;
		mFieldWidth[eYearSepField] = commaWidth+spaceWidth;
		mFieldWidth[eHourField] = digit00Width;
		mFieldWidth[eHourSepField] = timeSepWidth;
		mFieldWidth[eMinuteField] = digit00Width;
		mFieldWidth[eMinuteSepField] = timeSepWidth;
		mFieldWidth[eSecondField] = digit00Width;
		mFieldWidth[eSecondSepField] = spaceWidth;
		mFieldWidth[eAMPMField] = amPmWidth;
	
		uint16_t	fieldX = spaceWidth; // gap
		for (uint16_t i = 0; i < eNumSubFields; i++)
		{
			uint8_t	ii = kFieldOrder[i];
			mFieldX[ii] = fieldX;
			fieldX += mFieldWidth[ii];
		}
		fieldX += spaceWidth; // gap
		mWidth = fieldX;
	}
}

/********************************** DrawSelf **********************************/
void XDateValueField::DrawSelf(void)
{
	mDirtyField = 0xFFFF;
	DrawFields();
}

/********************************* DrawFields *********************************/
void XDateValueField::DrawFields(void)
{
	XFont*	xFont = MakeFontCurrent();
	if (xFont)
	{
		DisplayController*	display = xFont->GetDisplay();
		uint16_t	textColor = mEnabled ? mFGColor :
						DisplayController::Calc565Color(mFGColor, 0, 184);
		xFont->SetTextColor(textColor);
		xFont->SetBGTextColor(mBGColor);
		display->SetFGColor(kSelectedFieldBGColor);
		display->SetBGColor(mBGColor);
		int16_t	x = 0;
		int16_t	y = 0;
		// Bug to fix: if mY's global value is zero, FillRect and
		// DrawRoundedRect will fail.
		LocalToGlobal(x, y);
		uint16_t	mask = 1;
		for (uint16_t i = 0; i < eNumSubFields; i++, mask <<= 1)
		{
			if (mDirtyField & mask)
			{
				if (mStepper)
				{
					if (i == mActiveField)
					{
						xFont->SetTextColor(XFont::eWhite);
						xFont->SetBGTextColor(kSelectedFieldBGColor);
						display->DrawRoundedRect(x+mFieldX[i], y-3, mFieldWidth[i], mHeight+4, 3);
					} else
					{
						display->FillRect(x+mFieldX[i], y-3, mFieldWidth[i], mHeight+4, mBGColor);
					}
				}
				xFont->DrawAligned(mSubFieldStrs[i], x+mFieldX[i], y, mFieldWidth[i],
														XFont::eAlignCenter, false);
				xFont->SetTextColor(textColor);
				xFont->SetBGTextColor(mBGColor);
			}
		}
		mDirtyField = 0;
	}
}

/********************************* MouseDown **********************************/
void XDateValueField::MouseDown(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	/*
	*	If there is an associated Stepper THEN
	*	there can be an active field.
	*/
	if (mStepper)
	{
		int16_t	x = inGlobalX;
		int16_t	y = inGlobalY;
		GlobalToLocal(x, y);
		uint16_t i = 0;
		for (; i <= eYearField; i++)
		{
			if (x < mFieldX[i] || x >= mFieldX[i]+mFieldWidth[i])
			{
				continue;
			}
			break;
		}
		/*
		*	If a valid field was selected AND
		*	it's not already selected
		*/
		if (i <= eYearField &&
			i != mActiveField)
		{
			uint8_t	oldActiveSubfield = mActiveField;
			mActiveField = i;
			if (oldActiveSubfield != eNoSubField)
			{
				mDirtyField |= _BV(oldActiveSubfield);
			}
			mDirtyField |= _BV(i);
			DrawFields();
		}
	}
}

/******************************** ValueIsValid ********************************/
bool XDateValueField::ValueIsValid(
	int32_t	inValue)
{
	//					1/1/2000				12/31/2099
	return((uint32_t)inValue >= 946684800U && (uint32_t)inValue <= 4102401600U);
}

/******************************* IncrementValue *******************************/
bool XDateValueField::IncrementValue(void)
{
	return(IncDecValue(true));
}

/******************************* DecrementValue *******************************/
bool XDateValueField::DecrementValue(void)
{
	return(IncDecValue(false));
}

/******************************** IncDecValue *********************************/
bool XDateValueField::IncDecValue(
	bool	inIncrement)
{
	switch (mActiveField)
	{
		case eYearField:
			// For this implementation the range is 2000 to 2099
			if (inIncrement)
			{
				mComponents.year++;
				if (mComponents.year > 2099)
				{
					mComponents.year = 2000;
				}
			} else
			{
				mComponents.year--;
				if (mComponents.year < 2000)
				{
					mComponents.year = 2099;
				}
			}
			if (mComponents.day > 28)
			{
				uint8_t	daysInMonth = UnixTime::DaysInMonthForYear(mComponents.month, mComponents.year);
				if (mComponents.day > daysInMonth)
				{
					mComponents.day = daysInMonth;
					mDirtyField |= _BV(eDayField);
				}
			}
			mDirtyField |= _BV(eYearField);
			break;
		case eMonthField:
			if (inIncrement)
			{
				mComponents.month++;
				if (mComponents.month > 12)
				{
					mComponents.month = 1;
				}
			} else if (mComponents.month > 1)
			{
				mComponents.month--;
			} else
			{
				mComponents.month = 12;
			}
			if (mComponents.day > 28)
			{
				uint8_t	daysInMonth = UnixTime::DaysInMonthForYear(mComponents.month, mComponents.year);
				if (mComponents.day > daysInMonth)
				{
					mComponents.day = daysInMonth;
					mDirtyField |= _BV(eDayField);
				}
			}
			mDirtyField |= _BV(eMonthField);
			break;
		case eDayField:
			if (inIncrement)
			{
				mComponents.day++;
				if (mComponents.day > 28 &&
					mComponents.day > UnixTime::DaysInMonthForYear(mComponents.month, mComponents.year))
				{
					mComponents.day = 1;
				}
			} else if (mComponents.day == 1)
			{
				mComponents.day = UnixTime::DaysInMonthForYear(mComponents.month, mComponents.year);
			} else
			{
				mComponents.day--;
			}

			mDirtyField |= _BV(eDayField);
			break;
		case eHourField:
		{
			bool	isPM = mComponents.hour >= 12;
			if (inIncrement)
			{
				mComponents.hour++;
				if (mComponents.hour > 23)
				{
					mComponents.hour = 0;
				}
			} else if (mComponents.hour > 0)
			{
				mComponents.hour--;
			} else
			{
				mComponents.hour = 23;
			}
			mDirtyField |= _BV(eHourField);
			if (isPM != (mComponents.hour >= 12))
			{
				mDirtyField |= _BV(eAMPMField);
			}
			break;
		}
		case eMinuteField:
			if (inIncrement)
			{
				mComponents.minute++;
				if (mComponents.minute > 59)
				{
					mComponents.minute = 0;
				}
			} else if (mComponents.minute > 0)
			{
				mComponents.minute--;
			} else
			{
				mComponents.minute = 59;
			}
			mDirtyField |= _BV(eMinuteField);
			break;
		case eSecondField:
			if (inIncrement)
			{
				mComponents.second++;
				if (mComponents.second > 59)
				{
					mComponents.second = 0;
				}
			} else if (mComponents.second > 0)
			{
				mComponents.second--;
			} else
			{
				mComponents.second = 59;
			}
			mDirtyField |= _BV(eSecondField);
			break;
		case eAMPMField:
			if (mComponents.hour >= 12)
			{
				mComponents.hour -= 12;
			} else
			{
				mComponents.hour += 12;
			}
			mDirtyField |= _BV(eAMPMField);
			break;
	}
	if (mDirtyField)
	{
		UpdateStringsFromComponents();
		mValue = UnixTime::FromComponents(mComponents);
		DrawFields();
	}
	return(mDirtyField != 0);
}

/******************************** ValueChanged ********************************/
void XDateValueField::ValueChanged(
	bool	inUpdate)
{
	UpdateStringForValue();
	
	if (inUpdate)
	{
		DrawFields();
	}
}

/**************************** UpdateStringForValue ****************************/
void XDateValueField::UpdateStringForValue(void)
{
	UnixTime::ToComponents((time32_t)mValue, mComponents);
	if (mLastValue)
	{
		UnixTime::SComponents	lastComponents;
		mDirtyField = 0;
		UnixTime::ToComponents((time32_t)mLastValue, lastComponents);

		if (lastComponents.second != mComponents.second)
		{
			mDirtyField |= _BV(eSecondField);
		}
		if (lastComponents.minute != mComponents.minute)
		{
			mDirtyField |= _BV(eMinuteField);
		}
		if (lastComponents.hour != mComponents.hour)
		{
			mDirtyField |= _BV(eHourField);
			if ((lastComponents.hour >= 12) != (mComponents.hour >= 12))
			{
				mDirtyField |= _BV(eAMPMField);
			}
		}
		if (lastComponents.day != mComponents.day)
		{
			mDirtyField |= _BV(eDayField);
		}
		if (lastComponents.month != mComponents.month)
		{
			mDirtyField |= _BV(eMonthField);
		}
		if (lastComponents.year != mComponents.year)
		{
			mDirtyField |= _BV(eYearField);
		}
	} else
	{
		mDirtyField = 0xFFFF;
	}
	mLastValue = mValue;
	UpdateStringsFromComponents();
}

/************************ UpdateStringsFromComponents *************************/
void XDateValueField::UpdateStringsFromComponents(void)
{
	UnixTime::DecStrValue(mComponents.day, mDateStrings.day);
	mDateStrings.day[2] = 0;
#ifdef USE_MONTH_NAME
	UnixTime::CreateMonthStr(mComponents.month, mDateStrings.month);
#else
	UnixTime::DecStrValue(mComponents.month, mDateStrings.month);
	mDateStrings.month[2] = 0;
#endif
	UnixTime::Uint16ToDecStr(mComponents.year, mDateStrings.year);
	uint8_t	hour = mComponents.hour;
	/*
	*	If the hour is past noon THEN
	*	adjust the hour.
	*/
	if (hour > 12)
	{
		hour -= 12;
	} else if (hour == 0)
	{
		hour = 12;
	}
	UnixTime::DecStrValue(hour, mDateStrings.hour);
	mDateStrings.hour[2] = 0;
	UnixTime::DecStrValue(mComponents.minute, mDateStrings.minute);
	mDateStrings.minute[2] = 0;
	UnixTime::DecStrValue(mComponents.second, mDateStrings.second);
	mDateStrings.second[2] = 0;
	mSubFieldStrs[eAMPMField] = mComponents.hour >= 12 ? kPMStr : kAMStr;
}
