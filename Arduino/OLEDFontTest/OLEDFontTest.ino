#include <Wire.h>
#include "OLED_SSD1306.h"

#include "XFont.h"
XFont xFont;
#include "MyriadPro-Regular_24_1bRt.h"

// If you comment out DATA_ON_AT24C, rename MyriadPro-Regular_24_1bRta.h
// in the sketch folder to MyriadPro-Regular_24_1bRt.h
#define DATA_ON_AT24C	1
#ifdef DATA_ON_AT24C
#include "AT24C.h"
#include "AT24CDataStream.h"
const uint8_t kAT24CDeviceAddr = 0x50;
const uint8_t kAT24CDeviceCapacity = 32;	// Value at end of AT24Cxxx xxx/8
AT24C	eeprom(kAT24CDeviceAddr, kAT24CDeviceCapacity);
namespace MyriadPro_Regular_24_1bRt
{
	AT24CDataStream dataStream(&eeprom, 0, MyriadPro_Regular_24_1bRt::kGlyphDataLength);
	XFontR1BitDataStream xFontDataStream(&xFont, &dataStream);
	XFont::Font font(&fontHeader, charcodeRun, glyphDataOffset, &xFontDataStream);
}
#endif
//#define OLED_64x128	1
#ifdef OLED_64x128
OLED_SSD1306	display(0x3C, 64, 128);
#else
OLED_SSD1306	display(0x3C, 32, 128);
#endif

void setup(void)
{
	Wire.begin();

	xFont.SetDisplay(&display, &MyriadPro_Regular_24_1bRt::font);
	
#ifdef OLED_64x128
	display.begin(false);	// Init OLED rotation 0
#else
	display.begin(true);	// Init OLED rotation 180
#endif
	display.WakeUp();

	display.Fill();			// Clear the display
	display.MoveTo(0,0);
#ifdef OLED_64x128
	xFont.DrawStr("04:12:32 PM\n27°C", true);
#else
	xFont.DrawStr("04:15:32 PM", true);
#endif
}

void loop(void)
{
}


