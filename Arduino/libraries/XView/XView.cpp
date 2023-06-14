/*
*	XView.cpp, Copyright Jonathan Mackey 2023
*	Base class for a display view.
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
#ifndef __MACH__
#include <Arduino.h>
#else
#include <iostream>
#endif

/*********************************** XView ************************************/
XView::XView(
	int16_t			inX,
	int16_t			inY,
	uint16_t		inWidth,
	uint16_t		inHeight,
	uint16_t		inTag,
	XView*			inNextView,
	XView*			inSubViews,
	XView*			inSuperView,
	bool			inVisible,
	bool			inEnabled)
	: mX(inX), mY(inY), mTag(inTag),
	  mWidth(inWidth), mHeight(inHeight),
	  mNextView(inNextView), mSuperView(inSuperView),
	  mVisible(inVisible), mEnabled(inEnabled)
{
	SetSubViews(inSubViews);
}

/************************************ Draw ************************************/
/*
*	inX and inY are local.
*/
void XView::Draw(
	int16_t		inX,
	int16_t		inY,
	uint16_t	inWidth,
	uint16_t	inHeight)
{
	/*
	*	If this view is visible AND
	*	the area to be drawn intersects this view's bounds...
	*/
	if (mVisible &&
		mX + mWidth > inX &&
		inX + inWidth > mX &&
		mY + mHeight > inY &&
		inY + inHeight > mY)
	{
		DrawSelf();
		if (mSubViews)
		{
			mSubViews->Draw(inX-mX, inY-mY, inWidth, inHeight);
		}
	}
	if (mNextView)
	{
		mNextView->Draw(inX, inY, inWidth, inHeight);
	}
}

/******************************** SetSubViews *********************************/
void XView::SetSubViews(
	XView*	inSubView)
{
	/*
	*	If there are subviews THEN
	*	detach them from this superview.
	*/
	if (mSubViews)
	{
		mSubViews->SetSuperView();
	}
	mSubViews = inSubView;
	/*
	*	If there are subviews THEN
	*	attach them to this superview.
	*/
	if (mSubViews)
	{
		mSubViews->SetSuperView(this);
	}
}

/******************************** SetSuperView ********************************/
void XView::SetSuperView(
	XView*	inSuperView)
{
	mSuperView = inSuperView;
	if (mNextView)
	{
		mNextView->SetSuperView(inSuperView);
	}
}

/******************************** SetNextView *********************************/
void XView::SetNextView(
	XView*	inNextView)
{
	if (inNextView)
	{
		inNextView->mNextView = mNextView;
		mNextView = inNextView;
		inNextView->SetSuperView(mSuperView);
	}
}

#if 0
/******************************** PromoteView *********************************/
/*
*	Make inView the first subview.
*	inView must already be in the list of subviews.
*/
void XView::PromoteView(
	XView*	inView)
{
	/*
	*	If this view has subviews AND
	*	inView isn't already the first subview THEN
	*	make inView the first in the list of subviews.
	*/
	if (mSubViews &&
		inView != mSubViews)
	{
		XView*	previousView = nullptr;
		for (XView*	thisView = mSubViews; thisView; thisView = thisView->mNextView)
		{
			if (thisView != inView)
			{
				previousView = thisView;
				continue;
			}
			/*
			*	previousView must point to a view (no need to test for null)
			*	Remove inView from the chain then make it the first subview.
			*/
			previousView->mNextView = inView->mNextView;
			inView->mNextView = mSubViews;
			mSubViews = inView;
		}
	}
}

/********************************* DemoteView *********************************/
/*
*	Make inView the last subview.
*	inView must already be in the list of subviews.
*/
void XView::DemoteView(
	XView*	inView)
{
	/*
	*	If this view has subviews AND
	*	inView isn't already the last view THEN
	*	make inView the last in the list of subviews.
	*/
	if (mSubViews &&
		inView->mNextView)
	{
		XView*	previousView = nullptr;
		for (XView*	thisView = mSubViews; thisView; thisView = thisView->mNextView)
		{
			if (thisView != inView)
			{
				previousView = thisView;
				continue;
			}
			/*
			*	If inView isn't the first subview THEN
			*	remove inView from the chain
			*/
			if (previousView)
			{
				previousView->mNextView = inView->mNextView;
			/*
			*	Else, inView is the first subview.
			*	remove inView from the chain
			*/
			} else
			{
				mSubViews = inView->mNextView;
			}
			previousView = thisView = inView->mNextView;
			inView->mNextView = nullptr;
		}
		/*
		*	If at this point inView->mNextView is null, it means that inView
		*	has been removed from the chain.
		*	Make inView the last in the chain.
		*/
		if (inView->mNextView == nullptr)
		{
			previousView->mNextView = inView;
		}
	}
}

/******************************** GetRootView *********************************/
/*
*	Walk up the superview tree till the superview with no superview is hit.
*/
XView* XView::GetRootView(void)
{
	if (mSuperView)
	{
		return(mSuperView->GetRootView());
	}
	return(this);
}
#endif

/******************************* LocalToGlobal ********************************/
void XView::LocalToGlobal(
	int16_t&	ioX,
	int16_t&	ioY) const
{
	ioX += mX;
	ioY += mY;
	if (mSuperView)
	{
		mSuperView->LocalToGlobal(ioX, ioY);
	}
}

/******************************* GlobalToLocal ********************************/
void XView::GlobalToLocal(
	int16_t&	ioX,
	int16_t&	ioY) const
{
	if (mSuperView)
	{
		mSuperView->GlobalToLocal(ioX, ioY);
	}
	ioX -= mX;
	ioY -= mY;
}

/********************************** HitSelf ***********************************/
/*
*	inX and inY are local to the view.
*
*	Added for views that aren't rectangular, and/or views that want to capture
*	all hits.
*/
bool XView::HitSelf(
	int16_t	inLocalX,
	int16_t	inLocalY)
{
	return(	inLocalX >= 0 &&
			inLocalY >= 0 &&
			inLocalX < mWidth &&
			inLocalY < mHeight);
}

/********************************** HitTest ***********************************/
/*
*	inX and inY are local to the superview.
*/
XView* XView::HitTest(
	int16_t	inX,
	int16_t	inY)
{
	int16_t	localX = inX - mX;
	int16_t	localY = inY - mY;
	XView* hitView = nullptr;
	if (WantsClicks() &&
		HitSelf(localX, localY))
	{
		hitView = this;
		if (mSubViews)
		{
			XView* subHitView = mSubViews->HitTest(localX, localY);
			if (subHitView)
			{
				hitView = subHitView;
			}
		}
	} else if (mNextView)
	{
		hitView = mNextView->HitTest(inX, inY);
	}
	return(hitView);
}

/************************************ Show ************************************/
void XView::Show(void)
{
	if (!mVisible)
	{
		mVisible = true;
		DrawSelf();
	}
}

/************************************ Hide ************************************/
void XView::Hide(void)
{
	if (mSuperView &&
		mVisible)
	{
		mVisible = false;
		int16_t	x = mX;
		int16_t	y = mY;
		if (mSuperView != XRootView::GetInstance())
		{
			mSuperView->LocalToGlobal(x, y);
		}
		XView*	encompasingView = XRootView::GetInstance();
		/*
		*	Find the root view subview that completely encompasses this view.
		*	In some cases this may be the root view itself.
		*	The purpose of this is to limit the number of views that draw when
		*	a view is hidden.
		*/
		for (XView* thisView = XRootView::GetInstance()->mSubViews; thisView;
				thisView = thisView->mNextView)
		{
			if (!thisView->IsVisible() ||
				x < thisView->mX ||
				y < thisView->mY ||
				(x+mWidth) > (thisView->mX+thisView->mWidth) ||
				(y+mHeight) > (thisView->mY+thisView->mHeight))
			{
				continue;
			}
			encompasingView = thisView;
		}
		encompasingView->Draw(x, y, mWidth, mHeight);
	}
}

/*********************************** Enable ***********************************/
void XView::Enable(
	bool	inEnabled,
	bool	inUpdate)
{
	mEnabled = inEnabled;
	if (inUpdate)DrawSelf();
}

/******************************** ViewWithTag *********************************/
XView* XView::ViewWithTag(
	uint16_t	inTag)
{
	XView* viewWithTag = nullptr;
	if (inTag == mTag)
	{
		viewWithTag = this;
	} else
	{
		if (mNextView)
		{
			viewWithTag = mNextView->ViewWithTag(inTag);
		}
		if (!viewWithTag &&
			mSubViews)
		{
			viewWithTag = mSubViews->ViewWithTag(inTag);
		}
	}
	return(viewWithTag);
}

/******************************** HandleChange ********************************/
void XView::HandleChange(
	XView*		inChangedView,
	uint16_t	inAction)
{
	if (mSuperView)
	{
		mSuperView->HandleChange(inChangedView, inAction);
	}
}

