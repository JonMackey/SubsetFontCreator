/*
*	XRootView.h, Copyright Jonathan Mackey 2023
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
#ifndef XRootView_h
#define XRootView_h

#include "XView.h"

class DisplayController;

class XRootView : public XView
{
public:
							XRootView(
								XView*					inSubViews,
								XViewChangedDelegate*	inViewChangedDelegate = nullptr,
								DisplayController*		inDisplay = nullptr);
	virtual bool			WantsClicks(void) const
								{return(true);}
	virtual bool			HitSelf(
								int16_t					inLocalX,
								int16_t					inLocalY)
								{return(true);}
	virtual XView*			HitTest(
								int16_t					inX,
								int16_t					inY);
	void					SetDisplay(
								DisplayController*		inDisplay)
								{mDisplay = inDisplay;}
	DisplayController*		GetDisplay(void) const
								{return(mDisplay);}
	void					SetViewChangedDelegate(
								XViewChangedDelegate*	inViewChangedDelegate)
								{mViewChangedDelegate = inViewChangedDelegate;}
							/*
							*	The modal view is set by the view to become
							*	modal.  The view must restore the current value
							*	of mModalView when this view exists the modal
							*	state (e.g. when closing a modal dialog window
							*	or menu.)
							*/
	void					SetModalView(
								XView*					inModalView)
								{mModalView = inModalView;}
	XView*					ModalView(void) const
								{return(mModalView);}
	static XRootView*		GetInstance(void)
								{return(sInstance);}
protected:
	DisplayController*		mDisplay;
	XViewChangedDelegate*	mViewChangedDelegate;
	XView*					mModalView;
	static XRootView*		sInstance;

	virtual	void			HandleChange(
							XView*						inView,
							uint16_t					inAction = 0);
};

#endif // XRootView_h
