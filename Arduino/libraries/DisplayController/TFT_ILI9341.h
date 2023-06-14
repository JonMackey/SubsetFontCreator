/*
*	TFT_ILI9341.h, Copyright Jonathan Mackey 2022
*	Class to control an SPI TFT ILI9341/ILI9340 display controller.
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
#ifndef TFT_ILI9341_h
#define TFT_ILI9341_h

#include "TFT_ST77XX.h"

class TFT_ILI9341 : public TFT_ST77XX
{
public:
							TFT_ILI9341(
								pin_t					inDCPin,
								pin_t					inResetPin,	
								pin_t					inCSPin,
								pin_t					inBacklightPin = -1,
								uint16_t				inHeight = 320,
								uint16_t				inWidth = 240,
								bool					inCentered = false,
								bool					inIsBGR = true);
protected:
	virtual void			Init(void);
	virtual uint16_t		VerticalRes(void) const
								{return(320);}
	virtual uint16_t		HorizontalRes(void) const
								{return(240);}
	enum
	{
		eVMCTR2Cmd			= 0xC7, // VCOM Control 2
		ePWCTRACmd			= 0xCB, // Power control A
		ePWCTRBCmd			= 0xCF, // Power Control B
		eDRVTMCTRACmd		= 0xE8,	// Driver timing control A
		eDRVTMCTRBCmd		= 0xEA,	// Driver timing control B
		ePWONSEQCTRCmd		= 0xED,	// Power on sequence control
		eEN3GCmd			= 0xF2,	// Enable 3G
		ePUMPRTOCTRCmd		= 0xF7	// Pump ratio control
	};
};

#endif // TFT_ILI9341_h
