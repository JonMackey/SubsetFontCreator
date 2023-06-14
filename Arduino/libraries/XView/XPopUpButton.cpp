/*
*	XPopUpButton.cpp, Copyright Jonathan Mackey 2023
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

#include "XPopUpButton.h"
#include "XRootView.h"
#include "DisplayController.h"
#include "XMenuItem.h"
#ifdef __MACH__
#include <stdio.h>
#endif

static const struct SPopUpDim
{
	int16_t	size;
	int16_t	gap;
	int16_t	radius;
	int16_t	a[3];	// arrow x0, y0, y1 (x1 = 0)
} kPopUpDim[] = {
	{20, 5, 5, {2,2,4}},
	{16, 4, 4, {2,2,4}},
	{12, 4, 3, {1,1,2}}
};

/******************************** XPopUpButton ********************************/
XPopUpButton::XPopUpButton(
	int16_t							inX,
	int16_t							inY,
	uint16_t						inWidth,
	uint16_t						inHeight,
	uint16_t						inTag,
	XMenu*							inMenu,
	XView*							inNextView,
	XFont::Font*					inFont,
	XPopUpButton::EPopUpSize		inPopUpSize,
	uint16_t						inFGColor,
	uint16_t						inBGColor)
	: XMenuButton(inX, inY, inWidth, inHeight, inTag, inMenu, inNextView, nullptr, inFont,
	  inFGColor, inBGColor), mPopUpSize(inPopUpSize), mIgnoreNextMouseUp(false),
	  mOffsetForShowMenu(0)
{
	XFont*	xFont = mLabel.MakeFontCurrent();
	if (xFont)
	{
		const SPopUpDim& popUpDim = kPopUpDim[inPopUpSize];
		mLabel.MoveBy(popUpDim.gap, 0);
		mLabel.AdjustSize(-popUpDim.size-(popUpDim.gap*3), 0);
		mOffsetForShowMenu = xFont->GetFontHeader().height - popUpDim.gap;
	}
	if (inMenu)
	{
		inMenu->SetSuperViewAnchor(XMenu::eAnchorCentered);
		inMenu->SetAutoClearSelected(false);
		if (inMenu->GetMenuItems())
		{
			SelectMenuItem(inMenu->GetMenuItems());
		}
	}
}

/********************************** DrawSelf **********************************/
void XPopUpButton::DrawSelf(void)
{
	if (mVisible)
	{
		XFont*	xFont = mLabel.MakeFontCurrent();
		if (xFont)
		{
			DisplayController*	display = xFont->GetDisplay();
			uint16_t	iconColor, arrowColor;
			int16_t	x = 0;
			int16_t	y = 0;
			LocalToGlobal(x, y);
			const SPopUpDim& popUpDim = kPopUpDim[mPopUpSize];
			if (mEnabled)
			{
				iconColor = 0xFC01;
				arrowColor = 0xFFFF;
			} else
			{
				iconColor = 0xFFDF;
				arrowColor = 0xBDF7;
			}
			display->SetFGColor(0xB5B6);
			display->SetBGColor(mBGColor);
			display->DrawRoundedRect(x, y, mWidth, mHeight, mHeight/4, 255, true);
			display->SetFGColor(mFGColor);
			uint16_t	fillColor = display->DrawRoundedRect(x+1, y+1, mWidth-2, mHeight-2, mHeight/4, mState ? 200 : 255);
			int16_t	adjY = ((int16_t)mHeight - popUpDim.size)/2;
			if (adjY < 0)
			{
				adjY = 0;
			}
			y += adjY;
			display->SetFGColor(iconColor);
			display->SetBGColor(fillColor);
			x += (mWidth-popUpDim.size-popUpDim.gap);
			int16_t	radius = popUpDim.size/4;
			display->DrawRoundedRect(x, y, popUpDim.size, popUpDim.size, radius, 255);

			int16_t	halfWidth = popUpDim.size/2;
			display->SetFGColor(arrowColor);
			display->SetBGColor(iconColor);

			x+=(halfWidth-1);
			y+=(halfWidth-1);
			display->DrawLine(x-popUpDim.a[0], y-popUpDim.a[1], x, y-popUpDim.a[2], 2);
			x++;
			display->DrawLine(x+popUpDim.a[0], y-popUpDim.a[1], x, y-popUpDim.a[2], 2);
			display->FillRect(x-1, y-popUpDim.a[2], 1, 1, arrowColor);
			y++;
			display->DrawLine(x+popUpDim.a[0], y+popUpDim.a[1], x, y+popUpDim.a[2], 2);
			x--;
			display->DrawLine(x-popUpDim.a[0], y+popUpDim.a[1], x, y+popUpDim.a[2], 2);
			display->FillRect(x+1, y+popUpDim.a[2], 1, 1, arrowColor);

			mLabel.SetBGColor(fillColor);
			mLabel.DrawSelf();
		}
	}
}

/********************************* MouseDown **********************************/
void XPopUpButton::MouseDown(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	if (mState == eOn)
	{
		int16_t	localX = inGlobalX;
		int16_t	localY = inGlobalY;
		GlobalToLocal(localX, localY);
		/*
		*	If the anchoring menu button was hit THEN
		*	close the menu.
		*/
		if (XView::HitSelf(localX, localY))
		{
			SetState(eOff, false);
		} else
		{
			/*
			*	If the mouse down was within one of the menu items THEN
			*	highlight the menu item.
			*/
			if (mMenu->HitTest(inGlobalX, inGlobalY))
			{
				mMenu->MouseDown(inGlobalX, inGlobalY);
				// If a separator item or a disabled item was hit, ignore it.
				mIgnoreNextMouseUp = mMenu->GetSelectedItem(false) == nullptr;
			} else
			{
				SetState(eOff, false);
			}
		}
		if (mState == eOff)
		{
			DrawSelf();
			XRootView::GetInstance()->SetModalView(mSavedModalView);
			mSavedModalView = nullptr;
			mMenu->Hide();
		}
	/*
	*	Else, prepare to display the list of menu items.
	*	
	*/
	} else
	{
		SetState(eOn);
		/*
		*	mOffsetForShowMenu is a hack to get the menu text to align with
		*	this button's mLabel text.
		*/
		MoveBy(-mOffsetForShowMenu, 0);
		mMenu->Show();
		MoveBy(mOffsetForShowMenu, 0);
		mIgnoreNextMouseUp = true;
		mSavedModalView = XRootView::GetInstance()->ModalView();
		XRootView::GetInstance()->SetModalView(this);
	}
}

/********************************** MouseUp ***********************************/
void XPopUpButton::MouseUp(
	int16_t	inGlobalX,
	int16_t	inGlobalY)
{
	if (mIgnoreNextMouseUp)
	{
		mIgnoreNextMouseUp = false;
	} else
	{
		/*
		*	If the mouse up didn't occur over the selected item THEN
		*	clear the selection.
		*/
		mMenu->MouseUp(inGlobalX, inGlobalY);
		XMenuItem*	selectedItem = mMenu->GetSelectedItem(false);
		if (selectedItem)
		{
			SetState(eOff);
			SelectMenuItem(selectedItem);
			if (XRootView::GetInstance()->ModalView() == this)
			{
				XRootView::GetInstance()->SetModalView(mSavedModalView);
				mSavedModalView = nullptr;
			}
			mMenu->Hide();
		}
	}
}

/******************************* SelectMenuItem *******************************/
void XPopUpButton::SelectMenuItem(
	uint16_t	inTag)
{
	SelectMenuItem(mMenu->FindMenuItemWithTag(inTag));
}

/******************************* SelectMenuItem *******************************/
void XPopUpButton::SelectMenuItem(
	XMenuItem*	inMenuItem)
{
	mMenu->SelectMenuItem(inMenuItem);
	mLabel.SetString(inMenuItem ? inMenuItem->GetString() : nullptr);
}
