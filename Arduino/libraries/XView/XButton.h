/*
*	XButton.h, Copyright Jonathan Mackey 2023
*	Base class for a button.
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
#ifndef XButton_h
#define XButton_h

#include "XControl.h"
#include "XLabel.h"

class XButton : public XControl
{
public:
							XButton(
								int16_t					inX,
								int16_t					inY,
								uint16_t				inWidth,
								uint16_t				inHeight,  // 0 = auto
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								const char*				inString = nullptr,
								XFont::Font*			inFont = nullptr,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack,
								XFont::ETextAlignment	inTextAlignment = XFont::eAlignLeft);
	virtual void			Enable(
								bool					inEnabled=true,
								bool					inUpdate=true);
	void					SetTextAlignment(
								XFont::ETextAlignment	inTextAlignment);
	virtual void			SetWidth(
								uint16_t				inWidth);
	virtual void			SetSize(
								uint16_t				inWidth,
								uint16_t				inHeight);
	virtual void			AdjustSize(
								int16_t					inWidthAdj,
								int16_t					inHeightAdj);
	bool					HasString(void) const
								{return(mLabel.HasString());}
	virtual void			SetString(
								const char*				inString,
								bool					inUpdate = true);								
protected:
	XLabel		mLabel;	
	uint16_t	mFGColor;
	uint16_t	mBGColor;
};

#endif // XButton_h
