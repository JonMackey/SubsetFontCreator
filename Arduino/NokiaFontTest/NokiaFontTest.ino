#include <Arduino.h>
#include <SPI.h>
#include "LCD_PCD8544.h"
#include <Wire.h>

#include "XFont.h"
XFont xFont;
#include "MyriadPro-Regular_22_1bRt.h"

const uint8_t	kDCPin = 5;
const uint8_t	kResetPin = 6;
const uint8_t	kCSPin = 7;
LCD_PCD8544 display(kDCPin, kResetPin, kCSPin);

/********************************** setup *************************************/
void setup(void)
{
	display.begin();
	xFont.SetDisplay(&display, &MyriadPro_Regular_22_1bRt::font);
	display.Fill();
	display.MoveTo(0,0);
	xFont.DrawStr("80.2°F");
}


/********************************** loop ***********************************/
void loop()
{
}

