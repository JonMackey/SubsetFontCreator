/*
*	XPopUpButton.h, Copyright Jonathan Mackey 2023
*	A popup menu button.
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
#ifndef XPopUpButton_h
#define XPopUpButton_h

#include "XMenuButton.h"

class XPopUpButton : public XMenuButton
{
public:
	enum EPopUpSize
	{
		eLargePopUpSize,	// 20x20 icon
		eNormalPopUpSize,	// 16x16 icon
		eSmallPopUpSize		// 12x12 icon
	};
							XPopUpButton(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,	// Pass 0 to autosize height based on font
								uint16_t				inTag,
								XMenu*					inMenu,
								XView*					inNextView = nullptr,
								XFont::Font*			inFont = nullptr,
								EPopUpSize				inPopUpSize = eLargePopUpSize,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack);
	virtual void			DrawSelf(void);
	virtual void			MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	virtual void			MouseUp(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	void					SelectMenuItem(
								uint16_t				inTag);
protected:
	EPopUpSize	mPopUpSize;
	int16_t		mOffsetForShowMenu;
	bool		mIgnoreNextMouseUp;

	void					SelectMenuItem(
								XMenuItem*				inMenuItem);
};

#endif // XPopUpButton_h
