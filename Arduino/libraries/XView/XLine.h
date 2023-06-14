/*
*	XLine.h, Copyright Jonathan Mackey 2023
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
#ifndef XLine_h
#define XLine_h

#include "XView.h"
#include "XFont.h"

class XLine : public XView
{
public:
							XLine(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								uint16_t				inColor = 0xFFFF,
								bool					inEnabled = true);
	virtual void			DrawSelf(void);
	inline uint16_t			GetColor(void) const
								{return(mColor);}
	void					SetColor(
								uint16_t				inColor)
								{mColor = inColor;}
protected:
	uint16_t				mColor;
};

#endif // XLine_h
