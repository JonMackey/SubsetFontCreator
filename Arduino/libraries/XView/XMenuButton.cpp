/*
*	XMenuButton.cpp, Copyright Jonathan Mackey 2023
*	A button that anchors and manages a single menu.
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

#include "XMenuButton.h"
#include "XRootView.h"
#include "DisplayController.h"

/******************************** XMenuButton *********************************/
XMenuButton::XMenuButton(
	int16_t			inX,
	int16_t			inY,
	uint16_t		inWidth,
	uint16_t		inHeight,
	uint16_t		inTag,
	XMenu*			inMenu,
	XView*			inNextView,
	const char*		inString,
	XFont::Font*	inFont,
	uint16_t		inFGColor,
	uint16_t		inBGColor)
	: XButton(inX, inY, inWidth, inHeight, inTag, inNextView, inString, inFont,
	  inFGColor, inBGColor), mMenu(inMenu), mIgnoreNextMouseUp(false),
	  mSavedModalView(nullptr)
{
	mLabel.SetTextColor(0);
	if (inMenu)
	{
		inMenu->SetSuperView(this);
	}
}

/********************************** HitSelf ***********************************/
/*
*	inX and inY are local to the view.
*
*	When this menu button's menu is open, all hits, even out of bounds hits, are
*	taken as being a hit.
*
*	To accomplish this, when going from state off to on, the owning subview
*	order is modified so that this object is tested first.
*/
bool XMenuButton::HitSelf(
	int16_t	inLocalX,
	int16_t	inLocalY)
{
	return(mState == eOn || XView::HitSelf(inLocalX, inLocalY));
}


/********************************** DrawSelf **********************************/
void XMenuButton::DrawSelf(void)
{
	if (mVisible)
	{
		XFont*	xFont = mLabel.MakeFontCurrent();
		if (xFont)
		{
			const int16_t	kRadius = 7;
			DisplayController*	display = xFont->GetDisplay();
			int16_t	x = 0;
			int16_t	y = 0;
			LocalToGlobal(x, y);
			display->SetFGColor(mFGColor);
			display->SetBGColor(mBGColor);
			uint16_t	fillColor = display->DrawRoundedRect(x, y, mWidth, mHeight, kRadius, mState ? 200 : 255);
			/*
			*	If the string is the reserved character '.' THEN
			*	draw a centered vertical ellipsis.
			*/
			if (mLabel.GetString()[0] == '.')
			{
				uint16_t	quarterHeight = mHeight/4;
				y += (quarterHeight +  ((mHeight%4)/2));
				x += ((mWidth-2)/2);
				for (uint8_t i = 0; i < 3; i++, y+=quarterHeight)
				{
					display->FillRect(x, y, 2, 2, XFont::eBlack);
				}
			} else
			{
				mLabel.SetBGColor(fillColor);
				mLabel.DrawSelf();
			}
		}
	}
}

/********************************* MouseDown **********************************/
void XMenuButton::MouseDown(
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
		mSavedModalView = XRootView::GetInstance()->ModalView();
		XRootView::GetInstance()->SetModalView(this);
		mMenu->Show();
		mIgnoreNextMouseUp = true;
	}
}

/********************************** MouseUp ***********************************/
void XMenuButton::MouseUp(
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
		SetState(eOff);
		if (XRootView::GetInstance()->ModalView() == this)
		{
			XRootView::GetInstance()->SetModalView(mSavedModalView);
			mSavedModalView = nullptr;
		}
		mMenu->Hide();
		mMenu->MouseUp(inGlobalX, inGlobalY);
	}
}
