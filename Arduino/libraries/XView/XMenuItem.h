/*
*	XMenuItem.h, Copyright Jonathan Mackey 2023
*	Base class for a menu item.
*
*	XMenuItem is a single item within a menu.  An XMenuItem may contain a
*	submenu.
*
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
#ifndef XMenuItem_h
#define XMenuItem_h

#include <inttypes.h>
class XMenu;

class XMenuItem
{
	friend class XMenu;
public:
							XMenuItem(
								uint16_t				inTag,
								const char*				inString = nullptr,
								XMenuItem*				inNextItem = nullptr,
								XMenu*					inSubmenu = nullptr);
	inline uint16_t			Tag(void) const
								{return(mTag);}
	void					SetTag(
								uint16_t				inTag)
								{mTag = inTag;}
	const char*				GetString(void) const
								{return(mString);}
	void					SetString(
								const char*				inString)
								{mString = inString;}
	inline bool				IsSeparator(void) const
								{return(mString[0] == '-');}
	XMenu*					GetSubmenu(void)
								{return(mSubmenu);}
	enum XMenuItemState
	{
		eOff,
		eOn,
		eDisabled
	};
	XMenuItemState			GetState(void) const
								{return(mState);}
	void					SetState(
								XMenuItemState			inState)
								{mState = inState;}
	inline bool				IsEnabled(void) const
								{return(mState < eDisabled);}
	inline bool				IsOff(void) const
								{return(mState != eOn);}
	inline bool				IsOn(void) const
								{return(mState == eOn);}
protected:
	const char*		mString;
	uint16_t		mTag;
	XMenuItemState	mState;
	XMenu*			mSubmenu;
	XMenuItem*		mNextItem; // The next item should only be modified by XMenu
};

#endif // XMenuItem_h
