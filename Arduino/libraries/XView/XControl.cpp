/*
*	XControl.cpp, Copyright Jonathan Mackey 2023
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

#include "XControl.h"


/********************************** XControl **********************************/
XControl::XControl(
	int16_t		inX,
	int16_t		inY,
	uint16_t	inWidth,
	uint16_t	inHeight,
	uint16_t	inTag,
	XView*		inNextView,
	bool		inEnabled)
	: XView(inX, inY, inWidth, inHeight, inTag, inNextView),
	  mState(XControl::eOff), mAllowsTristate(false)
{
}

/********************************** SetState **********************************/
void XControl::SetState(
	XControlState	inState,
	bool			inUpdate)
{
	if (mState != inState)
	{
		mState = inState;
		if (inUpdate)DrawSelf();
		HandleChange(this, mState);
	}
}

/******************************* AllowsTristate *******************************/
/*
*	Tristate/mixed only has meaning for a few button subclasses such as radio
*	buttons and checkboxes.
*/
void XControl::AllowsTristate(
	bool	inAllowsTristate,
	bool	inUpdate)
{
	mAllowsTristate = inAllowsTristate;
	if (!inAllowsTristate &&
		mState == eMixed)
	{
		SetState(eOff, inUpdate);
	}
}