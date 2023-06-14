/*
*	XControl.h, Copyright Jonathan Mackey 2023
*	Base class for a control.
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
#ifndef XControl_h
#define XControl_h

#include "XView.h"

class XControl : public XView
{
public:
							XControl(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								bool					inEnabled = true);
								
	enum XControlState
	{
		eOff,				// 0000
		eOn,				// 0001
		eMixed,				// 0010 used by checkboxes and radios
		eDownOn	= 4			// 0100	used by steppers
	};
	XControlState			GetState(void) const
								{return(mState);}
	virtual void			SetState(
								XControlState			inState,
								bool					inUpdate = true);
	void					AllowsTristate(
								bool					inAllowsTristate = true,
								bool					inUpdate=true);
protected:
	XControlState				mState; // off, on, mixed
	bool						mAllowsTristate;
};
#endif // XControl_h
