/*
*	XStepper.h, Copyright Jonathan Mackey 2023
*	A value stepper.
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
#ifndef XStepper_h
#define XStepper_h

#include "XControl.h"
class XValueField;

class XStepper : public XControl
{
public:
							XStepper(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XValueField*			inValueField);
	virtual void			DrawSelf(void);
	virtual void			MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	virtual void			MouseUp(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
protected:
	XValueField*		mValueField;
	uint16_t			mRadius;
	
	enum XStepperPart
	{
		eNoButtonPart,		// 00
		eUpButtonPart,		// 01
		eDownButtonPart,	// 10
		eUpAndDownParts		// 11
	};
	void					DrawStepper(
								uint8_t					inPart = eUpAndDownParts);
	XStepperPart			GetPartHit(
								int16_t					inGlobalX,
								int16_t					inGlobalY) const;
};

#endif // XStepper_h
