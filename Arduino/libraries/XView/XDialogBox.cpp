/*
*	XDialogBox.cpp, Copyright Jonathan Mackey 2023
*
*	Class that draws a dialog box (modal)
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

#include "XDialogBox.h"
#include "XRootView.h"
#include "DisplayController.h"
static const int16_t	kDialogFrameGap = 20;
static const int16_t	kTitleBarHeight = 30;
static const int16_t	kSpaceBetweenButtons = 10;
static const int16_t	kVerticalItemSpace = 16;
static const int16_t	kCornerRadius = 10;
static const uint16_t	kSeparatorLineColor = 0xC638;

/********************************* XDialogBox *********************************/
XDialogBox::XDialogBox(
	XView*					inLayoutViews,
	uint16_t				inTag,
	XView*					inNextView,
	const char*				inOKButtonText,
	const char*				inCancelButtonText,
	const char*				inTitle,
	XFont::Font*			inFont,
	XViewChangedDelegate*	inViewChangedDelegate,
	uint16_t				inFGColor,
	uint16_t				inBGColor)
	: XView(0,0,0,0, inTag, inNextView, nullptr, nullptr, false), // visible = false
	  mAutoSizeApplied(false),
	  mOKButton(0,0,0,0,inTag+eOKTagOffset, inLayoutViews, inOKButtonText, inFont, 0xFFFF, inFGColor),
	  mCancelButton(0,0,0,0,inTag+eCancelTagOffset, &mOKButton, inCancelButtonText, inFont, 0xFFFF, inFGColor),
	  mTitleLabel(0,0,0,0,inTag+eTitleTagOffset, &mCancelButton, inTitle, inFont, nullptr, 0, inFGColor, XFont::eAlignCenter),
	  mTitleLine(0,0,0,0,inTag+eTitleLineTagOffset,&mTitleLabel, kSeparatorLineColor),
	  mFGColor(inFGColor), mBGColor(inBGColor), mSavedModalView(nullptr),
	  mViewChangedDelegate(inViewChangedDelegate), mValidatorDelegate(nullptr),
	  mMinDialogWidth(0), mMinDialogHeight(0), mMinButtonWidth(80)
{
	SetSubViews(&mTitleLine);
}

/********************************** AutoSize **********************************/
/*
*	Autosizing is only applied once.  The layout views passed to the constructor
*	are used to calculate the size of the dialog.
*/
void XDialogBox::AutoSize(void)
{
	if (!mAutoSizeApplied)
	{
		XFont* xFont = mTitleLabel.MakeFontCurrent();
		if (xFont)
		{
			mAutoSizeApplied = true;
			DisplayController*	display = xFont->GetDisplay();
			/*
			*	Calculate the height and width by calculating the extent of the
			*	layout views, not including the title and Cancel/OK buttons.
			*/
			int16_t	width = 0;
			int16_t	height = 0;
			int16_t	yOffset = kDialogFrameGap;
			if (mTitleLabel.HasString())
			{
				yOffset+=kTitleBarHeight;
			}
			// mOKButton's next view is the start of the layout views passed
			// in the constructor.
			//
			// No validation is done to see if the dialog will fit on the display.
			// Any changes to the height or width to get the dialog to fit would
			// most likely not be acceptable anyway and would need to be modified
			// manually.
			for (XView* thisView = mOKButton.NextView(); thisView;
					thisView = thisView->NextView())
			{
				int16_t	thisRight = thisView->X() + thisView->Width();
				int16_t	thisBottom = thisView->Y() + thisView->Height();
				thisView->MoveBy(kDialogFrameGap, yOffset);
				if (thisRight > width)
				{
					width = thisRight;
				}
				if (thisBottom > height)
				{
					height = thisBottom;
				}			
			}
			mWidth = width + (kDialogFrameGap*2);
			mHeight = height + yOffset + kDialogFrameGap;
		
			if (mMinButtonWidth > mOKButton.Width())
			{
				mOKButton.SetWidth(mMinButtonWidth);
			}
			if (mOKButton.Width() > mCancelButton.Width())
			{
				mCancelButton.SetWidth(mOKButton.Width());
			} else
			{
				mOKButton.SetWidth(mCancelButton.Width());
			}
			if (mMinDialogWidth > mWidth)
			{
				mWidth = mMinDialogWidth;
			}
			if (mMinDialogHeight > mHeight)
			{
				mHeight = mMinDialogHeight;
			}
			
			/*
			*	If the OK button will be visible/used THEN
			*	increase the size of the dialog box and place it in the lower right
			*	corner of the dialog.
			*	Note: Having just a cancel button and no OK button is not supported.
			*/
			if (mOKButton.HasString())
			{
				mHeight += (mOKButton.Height() + kVerticalItemSpace);
				int16_t	minWidth = mOKButton.Width() + (kDialogFrameGap*2);
				if (mCancelButton.HasString())
				{
					minWidth += (mCancelButton.Width() + kSpaceBetweenButtons);
				} else
				{
					mCancelButton.SetVisible(false);
				}
				if (mWidth < minWidth)
				{
					mWidth = minWidth;
				}
				mOKButton.SetOrigin(mWidth - mOKButton.Width() - kDialogFrameGap,
									mHeight - mOKButton.Height() - kDialogFrameGap);
				if (mCancelButton.HasString())
				{
					mCancelButton.SetOrigin(mOKButton.X() - mCancelButton.Width() - kSpaceBetweenButtons,
												mOKButton.Y());
				}
			} else
			{
				mOKButton.SetVisible(false);
				mCancelButton.SetVisible(false);
			}
			/*
			*	If there's a title THEN
			*	place it (now that the final dialog width has been set.)
			*/
			if (mTitleLabel.HasString())
			{
				mTitleLabel.SetSize(mWidth, xFont->GetFontHeader().height);
				mTitleLabel.SetOrigin(0, (kTitleBarHeight - (int16_t)(xFont->GetFontHeader().height))/2);
				mTitleLine.SetSize(mWidth-2, 1);
				mTitleLine.SetOrigin(1, kTitleBarHeight-1);
			} else
			{
				mTitleLine.SetVisible(false);
			}
		
			/*
			*	Calculate the origin based on the height and width.
			*	The standard 1/4 vertical placement dialog/alert position is used.
			*/
			mX = (display->GetColumns() - mWidth)/2;
			mY = (display->GetRows() - mHeight)/4;
		}
	}
}

/********************************** HitSelf ***********************************/
/*
*	inX and inY are local to the view.
*
*	When this dialog is showing, all hits, even out of bounds hits, are
*	taken as being a hit (i.e. modal.)
*
*	To accomplish this the root view's "modal view" is set to this dialog.
*/
bool XDialogBox::HitSelf(
	int16_t	inLocalX,
	int16_t	inLocalY)
{
	return(mVisible);
}

/************************************ Show ************************************/
void XDialogBox::Show(void)
{
	if (!mVisible)
	{
		mSavedModalView = XRootView::GetInstance()->ModalView();
		XRootView::GetInstance()->SetModalView(this);
		mVisible = true;
		AutoSize();
		Draw(0, 0, 0x7FFF, 0x7FFF);
	}
}

/******************************** HandleChange ********************************/
void XDialogBox::HandleChange(
	XView*		inChangedView,
	uint16_t	inAction)
{
	if (inAction == XControl::eOff &&
		(inChangedView == &mOKButton ||
		inChangedView == &mCancelButton))
	{
		if (inChangedView == &mCancelButton ||
			(mValidatorDelegate == nullptr || mValidatorDelegate->ValuesAreValid(this)))
		{
			XRootView::GetInstance()->SetModalView(mSavedModalView);
			mSavedModalView = nullptr;
			Hide();
			if (mViewChangedDelegate)
			{
				mViewChangedDelegate->HandleViewChange(inChangedView, inAction);
			}
		}
	} else if (mViewChangedDelegate)
	{
		mViewChangedDelegate->HandleViewChange(inChangedView, inAction);
	} else
	{
		XView::HandleChange(inChangedView, inAction);
	}
}

/********************************** DoCancel **********************************/
void XDialogBox::DoCancel(void)
{
	if (IsVisible())
	{
		HandleChange(&mCancelButton, XControl::eOff);
	}
}

/************************************ Draw ************************************/
/*
*	For a dialog, inX and inY are global because they are placed within the
*	root view.
*/
void XDialogBox::Draw(
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
		/*
		*	If the edge of the dialog is clipped THEN
		*	draw the entire background.
		*/
		if (inX < mX ||
			inY < mY ||
			inX+inWidth > mX+mWidth ||
			inY+inHeight > mY+mHeight)
		{
			DrawSelf();
			// Because the entire background is being drawn, make sure
			// all of the sub views are drawn, not just the views in the
			// original area passed.
			inX = inY = 0;
			inWidth = inHeight = 0x7FF;
		/*
		*	Else, the area being redrawn doesn't intersect, and is completely
		*	within the dialog frame.  Just fill the area to be redrawn with
		*	the display color.
		*/
		} else
		{
			XFont*	xFont = mTitleLabel.MakeFontCurrent();
			if (xFont)
			{
				xFont->GetDisplay()->FillRect(inX, inY, inWidth, inHeight, mFGColor);
			}
		}
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

/********************************** DrawSelf **********************************/
void XDialogBox::DrawSelf(void)
{
	XFont*	xFont = mTitleLabel.MakeFontCurrent();
	if (xFont)
	{
		DisplayController*	display = xFont->GetDisplay();
		display->SetFGColor(0xB5B6);	// DialogBox frame outline color
		display->SetBGColor(mBGColor);
		display->DrawRoundedRect(mX, mY, mWidth, mHeight, kCornerRadius, 255, true);
		display->SetFGColor(mFGColor);
		display->DrawRoundedRect(mX+1, mY+1, mWidth-2, mHeight-2, kCornerRadius);
	}
}

