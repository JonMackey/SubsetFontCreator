/*
*	XLine.cpp, Copyright Jonathan Mackey 2023
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

#include "XLine.h"
#include "XRootView.h"
#include "DisplayController.h"

/*********************************** XLine ***********************************/
XLine::XLine(
	int16_t					inX,
	int16_t					inY,
	uint16_t				inWidth,
	uint16_t				inHeight,
	uint16_t				inTag,
	XView*					inNextView,
	uint16_t				inColor,
	bool					inEnabled)
	: XView(inX, inY, inWidth, inHeight, inTag, inNextView, nullptr,
		nullptr, true, inEnabled),  mColor(inColor)
{
}

/********************************** DrawSelf **********************************/
void XLine::DrawSelf(void)
{
	DisplayController*	display = XRootView::GetInstance()->GetDisplay();
	if (display)
	{
		int16_t	x = 0;
		int16_t	y = 0;
		LocalToGlobal(x, y);
		uint16_t	color = mEnabled ? mColor :
						DisplayController::Calc565Color(mColor, 0, 184);
		display->FillRect(x, y, mWidth, mHeight, color);
	}
}
