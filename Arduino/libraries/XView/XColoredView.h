/*
*	XColoredView.h, Copyright Jonathan Mackey 2023
*
*	Class that draws/fills a background.
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
#ifndef XColoredView_h
#define XColoredView_h

#include "XView.h"

class DisplayController;

class XColoredView : public XView
{
public:
							XColoredView(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								XView*					inSubViews = nullptr,
								XView*					inSuperView = nullptr,
								bool					inVisible = true,
								bool					inEnabled = true,
								uint16_t				inColor =  0);
	virtual void			Draw(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight);
protected:
	uint16_t	mColor;
};

#endif // XColoredView_h
