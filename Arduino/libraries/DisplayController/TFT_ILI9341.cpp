/*
*	TFT_ILI9341.cpp, Copyright Jonathan Mackey 2022
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
#include <SPI.h>
#include "TFT_ILI9341.h"
#include <DataStream.h>
#include "Arduino.h"


/******************************** TFT_ILI9341 *********************************/
TFT_ILI9341::TFT_ILI9341(
	pin_t		inDCPin,
	pin_t		inResetPin,	
	pin_t		inCSPin,
	pin_t		inBacklightPin,
	uint16_t	inHeight,
	uint16_t	inWidth,
	bool		inCentered,
	bool		inIsBGR)
	: TFT_ST77XX(inDCPin, inResetPin, inCSPin, inBacklightPin, inHeight, inWidth,
				inCentered, inIsBGR, true)
{
}

/************************************ Init ************************************/
void TFT_ILI9341::Init(void)
{
	// These settings were copied from Adafruit_ILI9341.cpp
	static const uint8_t initCmds[] PROGMEM
	{
		0xEF, 3,			// ???
		0x03, 0x80, 0x02,
		ePWCTRBCmd, 3,		// Power Control B
		0x00, 0xC1, 0x30,
		ePWONSEQCTRCmd, 4,	// Power on sequence control 
		0x64, 0x03, 0x12, 0x81,
		eDRVTMCTRACmd, 3,	// Driver timing control A
		0x85, 0x00, 0x78,
		ePWCTRACmd, 5,		// Power control A
		0x39, 0x2C, 0x00, 0x34, 0x02,
		ePUMPRTOCTRCmd,		// Pump ratio control
		1, 0x20,
		eDRVTMCTRBCmd, 2,
		0x00, 0x00,
		ePWCTR1Cmd, 1,		// Power control VRH[5:0]
		0x23,
		ePWCTR2Cmd, 1,		// Power control SAP[2:0];BT[3:0]
		0x10,
		eVMCTR1Cmd, 2,		// VCM control
		0x3e, 0x28,
		eVMCTR2Cmd, 1,		// VCM control2
		0x86,
		/*eMADCTLCmd, 1,		// Memory Access Control
		0x48,*/
		eVSCSADCmd, 1,		// Vertical scroll zero
		0x00,
		eCOLMODCmd, 1, 		// Interface Pixel Format
		0x55,
		eFRMCTR1Cmd, 2,		// Frame Rate Control (In normal mode/ Full colors)
		0x00, 0x18,
		eDISSET5Cmd, 3,		// Display Function Control
		0x08, 0x82, 0x27,
		eEN3GCmd, 1,		// 3Gamma Function Disable
		0x00,
		eGAMSETCmd, 1,		// Gamma curve selected
		0x01,
		/*eGMCTRP1Cmd, 15,  // Set Gamma
		0x0F, 0x31, 0x2B, 0x0C,
		0x0E, 0x08, 0x4E, 0xF1,
		0x37, 0x07, 0x10, 0x03,
		0x0E, 0x09, 0x00,
		eGMCTRN1Cmd, 15,  // Set Gamma
		0x00, 0x0E, 0x14, 0x03,
		0x11, 0x07, 0x31, 0xC1,
		0x48, 0x08, 0x0F, 0x0C,
		0x31, 0x36, 0x0F,*/
		eSLPOUTCmd, 0x80,                // Exit Sleep
		eDISPONCmd, 0x80,                // Display on
		0                                // End of list
	};
	
	BeginTransaction();
	// Init does a hardware or software reset depending on whether the reset
	// pin is defined followed by a Wake command.
	TFT_ST77XX::Init();
	WriteCmds(initCmds);
	EndTransaction();
}
