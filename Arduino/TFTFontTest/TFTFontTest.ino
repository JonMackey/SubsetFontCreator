#include <SPI.h>
//#define USE_ST7789	1
#ifdef USE_ST7789
#include "TFT_ST7789.h"
#else
#include "TFT_ST7735S.h"
#endif

#include "XFont.h"
XFont xFont;
//#define USE_1Bit_FONT	1
#ifdef USE_1Bit_FONT
#include "MyriadPro-Regular_36_1b.h"
#else
#include "Brush Script_65.h"
#endif

const uint8_t kDCPin = 7;
const int8_t kResetPin = 8;
#ifdef USE_ST7789
const int8_t kCDPin = -1;
TFT_ST7789	display(kDCPin, kResetPin, kCDPin, 240, 240);
#else
const int8_t kCDPin = 9;
TFT_ST7735S	display(kDCPin, kResetPin, kCDPin, 161, 80);
#endif

void setup(void)
{
	SPI.begin();
#ifdef USE_1Bit_FONT
	xFont.SetDisplay(&display, &MyriadPro_Regular_36_1b::font);
	display.begin(0); // Init TFT
	display.Fill(0);
	xFont.DrawStr("04:12:32 PM\n26°C", true);
#else
	xFont.SetDisplay(&display, &Brush_Script_65::font);
	display.begin(1); // Init TFT
	display.Fill();
	display.MoveTo(0,0);
	xFont.SetTextColor(0xFFE0);
	xFont.DrawStr("Fly", true);
#endif
}

void loop(void)
{
}
