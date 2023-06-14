/*
*	XDialogBox.h, Copyright Jonathan Mackey 2023
*
*	Class that draws a dialog box
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
#ifndef XDialogBox_h
#define XDialogBox_h

#include "XLabel.h"
#include "XPushButton.h"
#include "XLine.h"

class XValidatorDelegate;

class XDialogBox : public XView
{
public:
							/*
							*	If any of the layout views can potentially
							*	generate a redraw (e.g. XPopUpButton), try to
							*	place all layout views at the same level.
							*	(i.e. part of the same next view chain.)
							*	Layout items placed within a separate view will
							*	force the entire view to redraw.
							*/
							XDialogBox(
								XView*					inLayoutViews,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								const char*				inOKButtonText = nullptr,
								const char*				inCancelButtonText = nullptr,
								const char*				inTitle = nullptr,
								XFont::Font*			inFont = nullptr,
								XViewChangedDelegate*	inViewChangedDelegate = nullptr,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack);
	virtual void			Draw(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight);
	virtual void			DrawSelf(void);
	virtual void			Show(void);
	void					DoCancel(void);
	virtual bool			WantsClicks(void) const
								{return(true);}
	virtual bool			HitSelf(
								int16_t					inLocalX,
								int16_t					inLocalY);
	void					SetViewChangedDelegate(
								XViewChangedDelegate*	inViewChangedDelegate)
								{mViewChangedDelegate = inViewChangedDelegate;}
	void					SetValidatorDelegate(
								XValidatorDelegate*	inValidatorDelegate)
								{mValidatorDelegate = inValidatorDelegate;}
	void					SetMinDialogSize(
								uint16_t				inMinDialogHeight = 100,
								uint16_t				inMinDialogWidth = 200)
								{mMinDialogHeight = inMinDialogHeight;
								 mMinDialogWidth = inMinDialogWidth;}
	void					SetMinButtonWidth(
								uint16_t				inMinButtonWidth = 80)
								{mMinButtonWidth = inMinButtonWidth;}

	enum ETagOffset
	{
		eTitleLineTagOffset = 96,
		eTitleTagOffset,
		eCancelTagOffset,
		eOKTagOffset
	};
protected:
	bool			mAutoSizeApplied;
	XLabel			mTitleLabel;
	XView*			mSavedModalView;
	XPushButton		mOKButton;
	XPushButton		mCancelButton;
	XLine			mTitleLine;
	uint16_t		mFGColor;
	uint16_t		mBGColor;
	uint16_t		mMinDialogWidth;
	uint16_t		mMinDialogHeight;
	uint16_t		mMinButtonWidth;
	XViewChangedDelegate*	mViewChangedDelegate;
	XValidatorDelegate*		mValidatorDelegate;

	virtual	void			HandleChange(
							XView*						inView,
							uint16_t					inAction = 0);
	void					AutoSize(void);
};

/*
*	XValidatorDelegate is a mixin class that handles dialog validation.
*	if ValuesAreValid() returns false the dialog will not be dismissed by
*	pressing the OK button.
*
*	Ex: 	class foo : public optionalSomeBase, public XValidatorDelegate
*			{
*			public:
*				foo(void);
*				bool ValuesAreValid(
*						XDialogBox*		inDialog);
*			}
*/
class XValidatorDelegate
{
public:
							XValidatorDelegate(void){}
	virtual bool			ValuesAreValid(
								XDialogBox*				inDialog) = 0;
};



#endif // XDialogBox_h
