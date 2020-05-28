/*******************************************************************************
	License
	****************************************************************************
	This program is free software; you can redistribute it
	and/or modify it under the terms of the GNU General
	Public License as published by the Free Software
	Foundation; either version 3 of the License, or
	(at your option) any later version.
 
	This program is distributed in the hope that it will
	be useful, but WITHOUT ANY WARRANTY; without even the
	implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public
	License for more details.
 
	Licence can be viewed at
	http://www.gnu.org/licenses/gpl-3.0.txt
//
	Please maintain this license information along with authorship
	and copyright notices in any redistribution of this code
*******************************************************************************/
/*
*	LCD_SD7567_Test.ino
*	Copyright (c) 2020 Jon Mackey
*
*	Serial commands:
*		C - Clear the display
*		F - Fill the display
*		I - Invert the display
*		E - Draw the next character(s) sent
*		D - Dump Glyph Header of the last character loaded/drawn.
*		0 - Move To line 0, column 0
*		S - Put display to sleep
*		W - Wakeup display
*/
#include <SPI.h>
#include "LCD_ST7567.h"
#include "MSPeriod.h"
#include "XFont.h"
XFont xFont;
#include "Tahoma_20_1bRt.h"

#define BAUD_RATE	19200

// The pins below are for an ATmega328p.
// Note that most controllers are 3v3 so use a 3v3 module OR level shift.
const uint8_t	kDCPin = 9;			// PB1
const int8_t	kResetPin = 8;		// PB0
const int8_t	kCSPin = SS;		// (Display select)


LCD_ST7567	display(kDCPin, kResetPin, kCSPin);

/********************************** setup *************************************/
void setup(void)
{
	Serial.begin(BAUD_RATE);
	SPI.begin();
	xFont.SetDisplay(&display, &Tahoma_20_1bRt::font);
	display.begin(0); // Init LCD, 0 = 0 degrees rotation, 2 = 180.  (90 & 270 not supported)
	display.Fill();
	xFont.DrawStr("ABCDEFGOMNZ\n0123456789");
}

/********************************* GetChar ************************************/
uint8_t GetChar(void)
{
	uint32_t	timeout = millis() + 1000;
	while (!Serial.available())
	{
		if (millis() < timeout)continue;
		return('\n');
	}
	return(Serial.read());
}

bool invert = false;
/*********************************** loop *************************************/
void loop(void)
{
	if (Serial.available())
	{
		switch (Serial.read())
		{
			case 'C':	// Clear
				display.Fill();
				break;
			case 'F':	// Fill
				display.Fill(0xFF);
				break;
			case 'I':	// Invert
				invert = !invert;
				display.Invert(invert);
				break;
			case 'E':	// Draw the next character sent
			{
				char lStr[32];
				uint8_t	i = 0;
				char thisChar = GetChar();
				while (thisChar != '\n' && i < 31)
				{
					lStr[i] = thisChar;
					thisChar = GetChar();
					i++;
				}
				lStr[i] = 0;
				Serial.println(lStr);
				xFont.DrawStr(lStr);
				break;
			}
			case 'D':	// Dump Glyph Header (last character loaded/drawn)
			{
				const GlyphHeader&	gh = xFont.Glyph();
				char charCode = (char)xFont.Charcode();
				Serial.print(F("\n\'"));
				Serial.print(charCode);
				Serial.print(F("\' {\n\tadvanceX = "));
				Serial.print(gh.advanceX);
				Serial.print(F("\n\tx = "));
				Serial.print(gh.x);
				Serial.print(F("\n\ty = "));
				Serial.print(gh.y);
				Serial.print(F("\n\trows = "));
				Serial.print(gh.rows);
				Serial.print(F("\n\tcolumns = "));
				Serial.print(gh.columns);
				Serial.print(F("\n}\n\n"));
				break;
			}
			case '0':	// Move To line 0
				xFont.MoveTo(0);
				break;
			case 'S':
				Serial.print(F("S\n"));
				display.Sleep();
				break;
			case 'W':
				Serial.print(F("W\n"));
				display.WakeUp();
				break;
		}
	}
}


