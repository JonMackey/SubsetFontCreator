/*
*	XMenuButton.h, Copyright Jonathan Mackey 2023
*	A button that anchors and manages a single menu.
*
*	At some point there may be a need for the class XMenuBar that will manage
*	a set of menus.
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
#ifndef XMenuButton_h
#define XMenuButton_h

#include "XButton.h"
#include "XMenu.h"

class XMenuButton : public XButton
{
public:
							XMenuButton(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XMenu*					inMenu,
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
	virtual bool			HitSelf(
								int16_t					inLocalX,
								int16_t					inLocalY);
protected:
	XMenu*		mMenu;
	XView*		mSavedModalView;
	bool		mIgnoreNextMouseUp;
};

#endif // XMenuButton_h
