/*
*	XFontDataStream.h, Copyright Jonathan Mackey 2023
*	Base class for a data stream used by XFont.
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
#ifndef XFontDataStream_h
#define XFontDataStream_h

#include "DataStream.h"
class XFont;

class XFontDataStream : public DataStream
{
public:
							XFontDataStream(
								XFont*					inXFont,
								DataStream*				inSourceStream);
	XFont*					GetXFont(void)
								{return(mXFont);}
	DataStream*				GetSourceStream(void)
								{return(mSourceStream);}
protected:
	XFont*		mXFont;
	DataStream*	mSourceStream;
};
#endif // XFontDataStream_h
