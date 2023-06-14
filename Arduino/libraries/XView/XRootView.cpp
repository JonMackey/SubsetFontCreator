/*
*	XRootView.cpp, Copyright Jonathan Mackey 2023
*
*	Class that draws/fills the display background.
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

#include "XRootView.h"
#include "DisplayController.h"

XRootView*		XRootView::sInstance;


/*********************************** XRootView ***********************************/
XRootView::XRootView(
	XView*					inSubViews,
	XViewChangedDelegate*	inViewChangedDelegate,
	DisplayController*		inDisplay)
	: XView(0, 0, 0, 0, 0, nullptr, inSubViews),
	  mDisplay(inDisplay),
	  mViewChangedDelegate(inViewChangedDelegate),
	  mModalView(nullptr)
{
	sInstance = this;
}

/******************************** HandleChange ********************************/
void XRootView::HandleChange(
	XView*		inChangedView,
	uint16_t	inAction)
{
	if (mViewChangedDelegate)
	{
		mViewChangedDelegate->HandleViewChange(inChangedView, inAction);
	}
}

/********************************** HitTest ***********************************/
/*
*	inX and inY are local to the superview.
*	Override to support modal views.
*/
XView* XRootView::HitTest(
	int16_t	inX,
	int16_t	inY)
{
	XView* hitView = mModalView;
	if (hitView == nullptr)
	{
		hitView = XView::HitTest(inX, inY);
	} else
	{
		hitView = mModalView->HitTest(inX, inY);
	}
	return(hitView);
}


