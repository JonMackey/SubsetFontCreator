/*
*	TFT_ILI9488.cpp, Copyright Jonathan Mackey 2022
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
#include <SPI.h>
#include "TFT_ILI9488.h"
#include <DataStream.h>
#include "Arduino.h"

// 1 of 32 mapped to 1 of 64.
// 5 bit color lookup to 6 bit by rounding numbers to closest 6 bit equivalent,
// then shifting the 6 bit value left 2 bits.  The shift by 2 makes the values
// suitable for ILI9488 memory.
const uint8_t TFT_ILI9488::k5To6Bit[] =
{
	0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
	0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78,
	0x80, 0x88, 0x94, 0x9C, 0xA4, 0xAC, 0xB4, 0xBC,
	0xC4, 0xCC, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xFC
};


/******************************** TFT_ILI9488 *********************************/
TFT_ILI9488::TFT_ILI9488(
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
void TFT_ILI9488::Init(void)
{
	// These settings were copied (mostly) from Adafruit_ILI9488.cpp
	static const uint8_t initCmds[] PROGMEM
	{
		eGMCTRP1Cmd, 15,	// 0xE0 Positive Gamma Correction
		0x00, 0x03, 0x09,
		0x08, 0x16, 0x0A,
		0x3F, 0x78, 0x4C,
		0x09, 0x0A, 0x08,
		0x16, 0x1A, 0x0F,
		eGMCTRN1Cmd, 15,	// 0xE1 Negative Gamma Correction
		0x00, 0x16, 0x19,
		0x03, 0x0F, 0x05,
		0x32, 0x45, 0x46,
		0x04, 0x0E, 0x0D,
		0x35, 0x37, 0x0F,
		ePWCTR1Cmd, 2,		// 0xC0 Power Control 1
		0x17, 0x15,
		ePWCTR2Cmd, 1,		// 0xC1 Power Control 2
		0x41,
		eVMCTR1Cmd, 3,		// 0xC5 VCM control
		0x00, 0x12, 0x80,
		//eINTMDCTRLCmd, 1,		// 0xB0 Interface Mode Control
		//0x00,				// SDO/MISO in use (00=default)
		//eINTMDCTRLCmd, 1,		// 0xB0 Interface Mode Control
		//0x80,				// SDO/MISO don't use
		eCOLMODCmd, 1,		// 0x3A Interface Pixel Format
		0x66,				// 666 18 bit
		eFRMCTR1Cmd, 1,		// 0xB1 Frame Rate Control
		0xA0,				// 60Hz
		eINVCTRCmd, 1,		// 0xB4 Display Inversion Control
		0x02,				// 2-Dot
		
		eDISSET5Cmd, 3,		// 0xB6 Display Function Control
		//0x08, 0x82, 0x27, //	27 = 320 lines
		0x02, 0x02, 0x3B,	//	3B = 480 lines (default)
		eSETIMGFUNCCmd, 1,	// 0xE9 Set Image Function
		0x00,				// Disable 24 bit data
		eADJCTR3Cmd, 4,		// 0xF7 Adjust Control 3
		0xA9, 0x51, 0x2C, 0x82, // D7 stream, loose
		//eSLPOUTCmd, 0x80, // 0x11 Exit Sleep
		eDISPONCmd, 0x80,	// 0x29 Display on
		0					// End of list
	};
	delay(150);
	BeginTransaction();
	// Init does a hardware or software reset depending on whether the reset
	// pin is defined followed by a Wake command.
	TFT_ST77XX::Init();
	WriteCmds(initCmds);
	EndTransaction();
}

/*
	Before optimizing FillPixels 320x480 pixels.
	Fill time = 2184036	(2.1s)
	
	After optimizing for 18-bit fill
	Fill time =  886859 (88.7ms)
	
	After optimizing for 3-bit fill
	Fill time = 153860	(15ms)
*/
/********************************* FillPixels *********************************/
/*
*	This is an override of the TFT_ST77XX routine.  This override converts
*	RGB565 to RGB666.  See k5To6Bit at top of file.
*/
void TFT_ILI9488::FillPixels(
	uint32_t	inPixelsToFill,
	uint16_t	inFillColor)
{
#if 1
	bool use3Bit = true;
	uint8_t fillColor = 0;
	/*
	*	This switch determines if 3-bit can be used.
	*/
	switch (inFillColor)	// BGR
	{
		case 0:			// 000	Black
			break;
		case 0x001F:	// 001	Red
			fillColor = 0b001001;	// Two 3-bit pixels of red
			break;
		case 0x07E0:	// 010	Green
			fillColor = 0b010010;
			break;
		case 0x07FF:	// 011	Yellow
			fillColor = 0b011011;
			break;
		case 0xF800:	// 100	Blue
			fillColor = 0b100100;
			break;
		case 0xF81F:	// 101	Magenta
			fillColor = 0b101101;
			break;
		case 0xFFFE:	// 110	Cyan
			fillColor = 0b110110;
			break;
		case 0xFFFF:	// 111	White
			fillColor = 0b111111;
			break;
		default:
		/*
		*	The default 18-bit fill is used when inFillColor is not 100% white,
		*	black, red, green, blue, cyan, magenta or yellow.  These are the
		*	only colors supported by the 3-bit pixel format.
		*/
		{
			use3Bit = false;
			// Note: I tried quadrupling the buffer size from 288 to 1152.
			//		 The time savings was negligable.
			uint8_t	buffer[288];	// 96 18-bit pixels (96 = 480/5)
			const uint32_t	kMaxPixels = sizeof(buffer)/3;
			uint8_t b = k5To6Bit[(inFillColor >> 11)];
			uint8_t g = (inFillColor >> 3) & 0xFC;
			uint8_t r = k5To6Bit[inFillColor & 0x1F];
			BeginTransaction();
			/*
			*	There are ways to do this more efficiently on a ESP32 or STM32 mcu.
			*	For ESP32 there's an option to copy a pattern.
			*	For STM32 there's an option to not receive on the transfer so you only
			*	have to fill the buffer once.
			*/
			while (inPixelsToFill)
			{
				uint32_t	bufferLen = inPixelsToFill > kMaxPixels ? kMaxPixels : inPixelsToFill;
				uint8_t*	bufferPtr = buffer;
				for (uint32_t i = 0; i < bufferLen; i++)
				{
					*(bufferPtr++) = b;
					*(bufferPtr++) = g;
					*(bufferPtr++) = r;
				}
				inPixelsToFill -= bufferLen;
				SPI.transfer(buffer, bufferLen*3);
			}
			EndTransaction();
			break;
		}
	}
	if (use3Bit)
	{
		/*
		*	Optimization for 3-bit pixels.
		*/
		uint8_t	buffer[240];	// 480 3-bit pixels
		bool	oddPixel = (inPixelsToFill & 1) != 0;
		uint32_t	pixelPairs = inPixelsToFill/2;
	
		BeginTransaction();
		/*
		*	If there are an odd number of pixels THEN
		*	write the odd pixel first.
		*
		*	Because in 3-bit format, each byte sent represents two pixels, the
		*	odd pixel needs to be written as an 18-bit pixel, followed by the
		*	pixel pairs.  This is done because the row range isn't set prior to
		*	calling FillPixels, so the memory address won't wrap back to the
		*	start of the block.  If the row range was set then you could just
		*	wrap around and write the first pixel twice.
		*/
		if (oddPixel)
		{
			buffer[0]=fillColor & 4 ? 0xFC : 0;
			buffer[1]=fillColor & 2 ? 0xFC : 0;
			buffer[2]=fillColor & 1 ? 0xFC : 0;
			SPI.transfer(buffer, 3);
		}
		if (pixelPairs)
		{
			WriteCmd(eCOLMODCmd);	// Set Interface Pixel Format
			SPI.transfer(0x61);		// to 3-bit
			WriteCmd(eWRMEMCCmd);	// Continue with write
			/*
			*	There are ways to do this more efficiently on a ESP32 or STM32 mcu.
			*	For ESP32 there's an option to copy a pattern.
			*	For STM32 there's an option to not receive on the transfer so you only
			*	have to fill the buffer once.
			*/
			while (pixelPairs)
			{
				uint32_t	bufferLen = pixelPairs > sizeof(buffer) ? sizeof(buffer) : pixelPairs;
				for (uint32_t i = 0; i < bufferLen; i++)
				{
					buffer[i] = fillColor;	// Lower 6 bits used (2 pixels)
				}
				pixelPairs -= bufferLen;
				SPI.transfer(buffer, bufferLen);
			}
			WriteCmd(eCOLMODCmd);	// Set Interface Pixel Format
			SPI.transfer(0x66);		// back to 18-bit
			//WriteCmd(eRAMWRCmd);
		}
		EndTransaction();
	}
#else
	// Least efficient
	uint8_t r = k5To6Bit[(inFillColor >> 11)];
	uint8_t g = (inFillColor >> 3) & 0xFC;
	uint8_t b = k5To6Bit[inFillColor & 0x1F];
	BeginTransaction();
	for (; inPixelsToFill; inPixelsToFill--)
	{
		SPI.transfer(r);
		SPI.transfer(g);
		SPI.transfer(b);
	}
	EndTransaction();
#endif
}

/******************************** StreamCopy **********************************/
void TFT_ILI9488::StreamCopy(
	DataStream* inDataStream,	// A 16 bit data stream
	uint16_t	inPixelsToCopy)
{
	BeginTransaction();
	uint16_t	buffer[96];	// WritePixelData's buffer holds 96 pixels.
	while (inPixelsToCopy)
	{
		uint16_t pixelsToWrite = inPixelsToCopy > 96 ? 96 : inPixelsToCopy;
		inPixelsToCopy -= pixelsToWrite;
		inDataStream->Read(pixelsToWrite, buffer);
		WritePixelData(buffer, pixelsToWrite);
	}
	EndTransaction();
}

/******************************** CopyPixels **********************************/
void TFT_ILI9488::CopyPixels(
	const void*		inPixels,
	uint16_t		inPixelsToCopy)
{
	BeginTransaction();
	WritePixelData((const uint16_t*)inPixels, inPixelsToCopy);
	EndTransaction();
}

/*
	Before optimizing WritePixelData of a very large 187 point font,
	writing "10:45" as the test text
	
	Draw time =  955596
	
	After optimizing
	Draw time =  416860
*/
/******************************* WritePixelData *******************************/
/*
*	inPixelData is an address in SRAM that points to RGB565 16 bit values.
*	The RGB565 values are converted to RGB666 values.
*/
void TFT_ILI9488::WritePixelData(
	const uint16_t* inPixelData,
	uint16_t		inDataLen) const
{
	if (inDataLen)
	{
#if 1
		uint8_t	buffer[288];	// 96 18-bit pixels (96 = 480/5)
		const uint32_t	kMaxPixels = sizeof(buffer)/3;

		while (inDataLen)
		{
			uint32_t	bufferLen = inDataLen > kMaxPixels ? kMaxPixels : inDataLen;
			uint8_t*	bufferPtr = buffer;
			for (uint32_t i = 0; i < bufferLen; i++)
			{
				uint16_t	rbg565Color = *(inPixelData++);
				*(bufferPtr++) = k5To6Bit[(rbg565Color >> 11)];
				*(bufferPtr++) = (rbg565Color >> 3) & 0xFC;
				*(bufferPtr++) = k5To6Bit[rbg565Color & 0x1F];
			}
			inDataLen -= bufferLen;
			SPI.transfer(buffer, bufferLen*3);
		}
#else
	// Least efficient
		const uint16_t* endData = &inPixelData[inDataLen];
		do
		{
			uint16_t	rbg565Color = *(inPixelData++);
			uint8_t r = k5To6Bit[(rbg565Color >> 11)];
			uint8_t g = (rbg565Color >> 3) & 0xFC;
			uint8_t b = k5To6Bit[rbg565Color & 0x1F];
			SPI.transfer(r);
			SPI.transfer(g);
			SPI.transfer(b);
		} while (inPixelData < endData);
#endif
	}
}


