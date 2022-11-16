/*
*	TFT_ILI9488.h, Copyright Jonathan Mackey 2022
*	Class to control an SPI TFT ILI9488 display controller.
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
#ifndef TFT_ILI9488_h
#define TFT_ILI9488_h

#include "TFT_ST77XX.h"

class TFT_ILI9488 : public TFT_ST77XX
{
public:
							TFT_ILI9488(
								pin_t					inDCPin,
								pin_t					inResetPin,	
								pin_t					inCSPin,
								pin_t					inBacklightPin = 0xFF,
								uint16_t				inHeight = 480,
								uint16_t				inWidth = 320,
								bool					inCentered = false,
								bool					inIsBGR = true);
	/*
	*	FillPixels: Sets a run of inPixelsToFill to inFillColor from the
	*	current position and column clipping.
	*/
	virtual void			FillPixels(
								uint32_t				inPixelsToFill,
								uint16_t				inFillColor);

	virtual void			StreamCopy(
								DataStream*				inDataStream,
								uint16_t				inPixelsToCopy);

	virtual void			CopyPixels(
								const void*				inPixels,
								uint16_t				inPixelsToCopy);
protected:
	static const uint8_t k5To6Bit[];
	enum
	{
		// See TFT_ST77XX.h for other values.
		eINTMDCTRLCmd		= 0xB0,	// Interface Mode Control
		eSETIMGFUNCCmd		= 0xE9,	// Set Image Function
		eADJCTR3Cmd			= 0xF7	// Adjust Control 3 
	};

	virtual void			Init(void);
	virtual uint16_t		VerticalRes(void) const
								{return(480);}
	virtual uint16_t		HorizontalRes(void) const
								{return(320);}
	void					WritePixelData(
								const uint16_t*			inData,
								uint16_t				inDataLen) const;
};

#endif // TFT_ILI9488_h
