/*
*	AT24CDataStream.h, Copyright Jonathan Mackey 2019
*	DataStream class for accessing data from the AT24C family of chips.
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
#ifndef AT24CDataStream_h
#define AT24CDataStream_h

#include "DataStream.h"
class AT24C;

class AT24CDataStream : public DataStreamImpl
{
public:
							AT24CDataStream(
								AT24C*					inAT24C,
								const void*				inStartAddress,
								uint32_t				inLength);
	virtual uint32_t		Read(
								uint32_t				inLength,
								void*					outBuffer);
	virtual uint32_t		Write(
								uint32_t				inLength,
								const void*				inBuffer);
protected:
	AT24C*		mAT24C;
};

#endif // AT24CDataStream_h