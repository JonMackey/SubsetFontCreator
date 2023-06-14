/*
*	XCheckboxButton.h, Copyright Jonathan Mackey 2023
*	A checkbox button.
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
#ifndef XCheckboxButton_h
#define XCheckboxButton_h

#include "XButton.h"

class XCheckboxButton : public XButton
{
public:
	enum ECheckSize
	{
		eLargeCheckSize,	// 18x18
		eNormalCheckSize,	// 16x16
		eSmallCheckSize,	// 14x14
		eMiniCheckSize		// 10x10
	};
							XCheckboxButton(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								const char*				inString = nullptr,
								XFont::Font*			inFont = nullptr,
								ECheckSize				inCheckSize = eLargeCheckSize,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack);
	virtual void			DrawSelf(void);
	virtual void			MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
protected:
	ECheckSize	mCheckSize;
};

#endif // XCheckboxButton_h
