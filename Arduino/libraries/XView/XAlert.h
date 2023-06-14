/*
*	XAlert.h, Copyright Jonathan Mackey 2023
*
*	Class that draws an alert dialog box
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
#ifndef XAlert_h
#define XAlert_h

#include "XDialogBox.h"

class XAlert : public XDialogBox
{
public:
							XAlert(
								uint16_t				inTag,
								XView*					inNextView = nullptr,
								const char*				inOKButtonText = nullptr,
								const char*				inCancelButtonText = nullptr,
								const char*				inTitle = nullptr,
								XFont::Font*			inFont = nullptr,
								XViewChangedDelegate*	inViewChangedDelegate = nullptr,
								uint16_t				inFGColor = XFont::eWhite,
								uint16_t				inBGColor =  XFont::eBlack);

	void					DoMessage(
								const char*				inMessage,
								uint16_t				inMessageTag = 0);
	uint16_t				MessageTag(void) const
								{return(mMessageTag);}
protected:
	XLabel			mMessageLabel;
	uint16_t		mMessageTag;
};

#endif // XAlert_h
