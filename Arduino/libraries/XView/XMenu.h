/*
*	XMenu.h, Copyright Jonathan Mackey 2023
*	Base class for a menu.
*
*	XMenu is a container of menu items.  It contains a one directional, null
*	terminated linked list.
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
#ifndef XMenu_h
#define XMenu_h

#include "XFont.h"
#include "XView.h"
class XMenuItem;

class XMenu : public XView
{
public:
							XMenu(
								uint16_t				inTag,
								XFont::Font*			inFont,
								XMenuItem*				inMenuItems = nullptr,
								XView*					inAnchorView = nullptr,
								uint16_t				inBGColor = 0);
	XMenuItem*				GetMenuItems(void)
								{return(mMenuItems);}
	XMenuItem*				FindMenuItemWithTag(
								uint16_t				inTag) const;
	void					InsertMenuItem(
								XMenuItem*				inMenuItem,
								uint16_t				inAfterTag = 0);
	XMenuItem*				RemoveMenuItemWithTag(
								uint16_t				inTag);
	uint16_t				GetWidestItemWidth(void);
	XMenuItem*				GetSelectedItem(
								bool					inClearSelection = false);
	const char*				GetSelectedItemStrPtr(void) const
								{return(mSelectedItemStrPtr);}
	void					SelectMenuItem(
								XMenuItem*				inMenuItem);
	void					ClearSelectedItem(void);
	uint16_t				GetItemCount(
								uint16_t*				outNumSeparators = nullptr) const;
	void					MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	void					MouseUp(
								int16_t					inGlobalX,
								int16_t					inGlobalY);
	virtual void			Hide(void);
	virtual void			Show(void);
	void					SetAutoClearSelected(
								bool					inAutoClearSelected)
								{mAutoClearSelected = inAutoClearSelected;}
								
	/*
	*	ESuperViewAnchor is a hint as to how the menu will be displayed relative
	*	to the superview.  The actual anchor point may be affected by the
	*	superview's placement and the size of the menu.
	*/
	enum ESuperViewAnchor
	{
		eAnchorToBottom,	// Default, used by XMenuButton
		eAnchorCentered		// Used by XPopUpButton
	};
	void					SetSuperViewAnchor(
								ESuperViewAnchor		inSuperViewAnchor)
								{mSuperViewAnchor = inSuperViewAnchor;}
protected:
	XFont::Font*			mFont;
	XMenuItem*				mMenuItems;
	XMenuItem*				mSelectedItem;
	const char*				mSelectedItemStrPtr;
	uint16_t				mBGColor;
	uint16_t				mTextInset;	// From outer frame
	uint16_t				mTextItemInsetY;	// From itemY
	uint16_t				mItemHeight;
	uint16_t				mHalfItemHeight;
	uint16_t				mQuarterItemHeight;
	uint16_t				mEighthItemHeight;
	uint16_t				mItemWidth;
	uint16_t				mItemFrameWidth;
	ESuperViewAnchor		mSuperViewAnchor;
	bool					mAutoClearSelected;

	int16_t					GetItemTop(
								XMenuItem*				inItem);
//	void					DrawItem(
//								XMenuItem*				inItem,
//								bool					inRedrawBG = false);
	uint16_t				DrawItem(
								int16_t					inGlobalY,
								XMenuItem*				inItem,
								bool					inRedrawBG = false);

};
#endif // XMenu_h
