/*
*	XPushButton.h, Copyright Jonathan Mackey 2023
*	A push button.
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
#ifndef XPushButton_h
#define XPushButton_h

#include "XButton.h"

class XPushButton : public XButton
{
public:
							XPushButton(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								const char*				inString = nullptr,
								XFont::Font*			inFont = nullptr,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack);
	virtual void			DrawSelf(void);
	virtual void			MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	virtual void			MouseUp(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
};

#endif // XPushButton_h
