/*
*	XView.h, Copyright Jonathan Mackey 2023
*
*	Base class for a display view.  This is designed for mcu's with at least
*	64KB of flash, and have a display that supports a touch screen.
*
*	All views are generally instantiated statically, frontmost first.  A pointer
*	to the view is passed to the contructor of the next view in the drawing
*	order either as the next view, or the first view of a subview.  In most
*	cases the superview is assigned dynamically within constructor of the
*	superview.  Exception would be for views contructed within another view,
*	such as the label of a button.
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
#ifndef XView_h
#define XView_h

#include <inttypes.h>

class XView
{
public:
							XView(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								XView*					inSubViews = nullptr,
								XView*					inSuperView = nullptr,
								bool					inVisible = true,
								bool					inEnabled = true);
	virtual void			Draw(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight);
	virtual void			DrawSelf(void){}
	virtual bool			WantsClicks(void) const
								{return(mVisible && mEnabled);}
	virtual void			MouseDown(
								int16_t					inGlobalX,
								int16_t					inGlobalY){}
	virtual void			MouseUp(
								int16_t					inGlobalX,
								int16_t					inGlobalY){}
	virtual XView*			HitTest(
								int16_t					inX,
								int16_t					inY);
	virtual bool			HitSelf(
								int16_t					inLocalX,
								int16_t					inLocalY);
	virtual XView*			ViewWithTag(
								uint16_t				inTag);
	virtual void			Hide(void);
							// SetVisible is Hide() without redrawing.
	void					SetVisible(
								bool					inVisible)
								{mVisible = inVisible;}
	virtual void			Show(void);
	virtual bool			IsVisible(void)
								{return(mVisible);}
	void					SetSubViews(
								XView*					inSubView = nullptr);
	inline XView*			SubViews(void) const
								{return(mSubViews);}
	void					SetSuperView(
								XView*					inSuperView = nullptr);
	inline XView*			SuperView(void) const
								{return(mSuperView);}
#if 0
	XView*					GetRootView(void);
	void					PromoteView(
								XView*					inView);
	void					DemoteView(
								XView*					inView);
#endif
	void					SetNextView(
								XView*					inNextView);
	inline XView*			NextView(void) const
								{return(mNextView);}
	inline uint16_t			Tag(void) const
								{return(mTag);}
	void					SetTag(
								uint16_t				inTag)
								{mTag = inTag;}
	void					SetOrigin(
								int16_t					inX,
								int16_t					inY)
								{mX = inX; mY = inY;}
	void					MoveBy(
								int16_t					inX,
								int16_t					inY)
								{mX += inX; mY += inY;}
	virtual void			SetSize(
								uint16_t				inWidth,
								uint16_t				inHeight)
								{mWidth = inWidth; mHeight = inHeight;}
	virtual void			AdjustSize(
								int16_t					inWidthAdj,
								int16_t					inHeightAdj)
								{mWidth += inWidthAdj; mHeight += inHeightAdj;}
	inline int16_t			X(void) const
								{return(mX);}
	inline int16_t			Y(void) const
								{return(mY);}
	inline uint16_t			Width(void) const
								{return(mWidth);}
	virtual void			SetWidth(
								uint16_t				inWidth)
								{mWidth = inWidth;}
	inline uint16_t			Height(void) const
								{return(mHeight);}
	virtual void			SetHeight(
								uint16_t				inHeight)
								{mHeight = inHeight;}
	void					LocalToGlobal(
								int16_t&				ioX,
								int16_t&				ioY) const;
	void					GlobalToLocal(
								int16_t&				ioX,
								int16_t&				ioY) const;
	bool					IsEnabled(void) const
								{return(mEnabled);}
	virtual void			Enable(
								bool					inEnabled=true,
								bool					inUpdate=true);
protected:
	bool			mEnabled;
	bool			mVisible;
	int16_t			mX;
	int16_t			mY;
	uint16_t		mWidth;
	uint16_t		mHeight;
	uint16_t		mTag;
	XView*			mSuperView;
	// mNextView and mSubViews are null terminated chains
	XView*			mNextView;	// At same level as this view
	XView*			mSubViews;	// First subview in chain of within this view
	
	/*
	*	The change walks up the superview hierarchy until a superview override
	*	of HandleChange(), handles the change.  The XRootView is the default
	*	handler of all changes.
	*/
	virtual	void			HandleChange(
								XView*					inView,
								uint16_t				inAction = 0);
};


/*
*	XViewChangedDelegate is a mixin class that handles changes to views.
*
*	Ex: 	class foo : public optionalSomeBase, public XViewChangedDelegate
*			{
*			public:
*				foo(void);
*				void HandleChange(
*						XView*		inView,
*						uint16_t	inAction);
*			}
*/
class XViewChangedDelegate
{
public:
							XViewChangedDelegate(void){}
	virtual void			HandleViewChange(
								XView*					inView,
								uint16_t				inAction) = 0;
};


#endif // XView_h
