/*
*	AT24CDataStream.cpp, Copyright Jonathan Mackey 2019
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
#include "AT24CDataStream.h"
#include "AT24C.h"

/****************************** AT24CDataStream *******************************/
AT24CDataStream::AT24CDataStream(
	AT24C*		inAT24C,
	const void*	inStartAddress,
	uint32_t	inLength)
	: DataStreamImpl(inStartAddress, inLength), mAT24C(inAT24C)
{
}

/************************************ Read ************************************/
uint32_t AT24CDataStream::Read(
	uint32_t	inLength,
	void*		outBuffer)
{
	uint32_t	bytesRead = mAT24C->Read((uint32_t)mCurrent, Clip(inLength), (uint8_t*)outBuffer);
	mCurrent+=bytesRead;
	return(bytesRead);
}

/*********************************** Write ************************************/
uint32_t AT24CDataStream::Write(
	uint32_t	inLength,
	const void*	inBuffer)
{
	// Space needs to be preallocated via the constructor, the end doesn't
	// automatically extend.
	uint32_t	bytesWritten = mAT24C->Write((uint32_t)mCurrent, Clip(inLength), (const uint8_t*)inBuffer);
	mCurrent+=bytesWritten;
	return(bytesWritten);
}

