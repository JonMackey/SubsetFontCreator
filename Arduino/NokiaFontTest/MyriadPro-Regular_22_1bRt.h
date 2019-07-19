// Subset font created by SubsetFontCreator
// For subset: "  -.09CCFF°°"

#ifndef MyriadPro_Regular_22_1bRt_h
#define MyriadPro_Regular_22_1bRt_h

#include "XFontGlyph.h"
#include "XFontR1BitDataStream.h"

namespace MyriadPro_Regular_22_1bRt
{
	const FontHeader	fontHeader PROGMEM =
	{
		1,		// version, currently version = 1
		1,		// oneBit, 1 = 1 bit per pixel, 0 = 8 bit (antialiased)
		1,		// rotated, glyph data is rotated (applies to 1 bit only)
		0,		// wideOffsets, 1 = 32 bit, 0 = 16 bit glyph data offsets
		0,		// monospaced, fixed width font (for this subset)
		17,		// ascent, font in pixels
		-6,		// descent, font in pixels
		26,		// height, font height (ascent+descent+leading) in pixels
		13,		// width, widest glyph advanceX within subset in pixels
		7,		// numCharcodeRuns
		16		// numCharCodes
	};
	
	const CharcodeRun	charcodeRun[] PROGMEM = // {start, entryIndex}, ...
	{
		{0x0020, 0}, {0x002D, 1}, {0x0030, 3}, {0x0043, 13}, {0x0046, 14},
		{0x00B0, 15}, {0xFFFF, 16}
	};
	
	const uint16_t	glyphDataOffset[] PROGMEM =
	{
		0x0000, 0x0005, 0x000B, 0x0012, 0x0028, 0x0037, 0x004D, 0x0063,
		0x007D, 0x0093, 0x00AB, 0x00C1, 0x00D7, 0x00EF, 0x0109, 0x011D,
		0x0126
	};
	
	const uint8_t	glyphData[] PROGMEM =
	{
		0x05, 0x00, 0x11, 0x00, 0x00, 0x07, 0x01, 0x0B, 0x01, 0x05, 0x1F, 0x05,
		0x01, 0x0E, 0x03, 0x03, 0xBE, 0x00, 0x0B, 0x01, 0x02, 0x0F, 0x09, 0xF8,
		0x0F, 0xFE, 0x9F, 0x01, 0x6C, 0x00, 0x3C, 0x00, 0x1C, 0x00, 0x1F, 0xC0,
		0x7D, 0x7C, 0xFC, 0x0F, 0x0B, 0x02, 0x02, 0x0F, 0x05, 0x04, 0x00, 0x03,
		0xC0, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x07, 0x0B, 0x01, 0x02, 0x0F, 0x09,
		0x02, 0x60, 0x01, 0xF8, 0x00, 0x7E, 0x80, 0x3F, 0xE0, 0x1E, 0x38, 0x1F,
		0x87, 0xFD, 0xC1, 0x38, 0x60, 0x0B, 0x01, 0x02, 0x0F, 0x09, 0x00, 0x20,
		0x01, 0xF0, 0x10, 0x78, 0x08, 0x3C, 0x0C, 0x1E, 0x07, 0xFF, 0xC6, 0x3C,
		0x7E, 0x00, 0x0E, 0x0B, 0x00, 0x02, 0x0F, 0x0B, 0x00, 0x06, 0x80, 0x03,
		0xF0, 0x01, 0xDC, 0x80, 0x63, 0xE0, 0x30, 0x1C, 0x18, 0xFE, 0xFF, 0xFF,
		0x7F, 0x00, 0x03, 0x80, 0x01, 0x0B, 0x01, 0x02, 0x0F, 0x09, 0x00, 0x20,
		0x3F, 0xF0, 0x1F, 0x78, 0x08, 0x3C, 0x04, 0x1E, 0x06, 0x0F, 0xC7, 0x06,
		0x3F, 0x03, 0x0F, 0x0B, 0x01, 0x02, 0x0F, 0x0A, 0xE0, 0x0F, 0xFC, 0x1F,
		0x13, 0xCC, 0x0C, 0x2C, 0x02, 0x1E, 0x03, 0x8F, 0x81, 0x87, 0x73, 0xC0,
		0x1F, 0x00, 0x03, 0x0B, 0x01, 0x02, 0x0F, 0x09, 0x03, 0x80, 0x01, 0xE0,
		0x00, 0x7C, 0x80, 0x37, 0xF0, 0x18, 0x1F, 0xEC, 0x01, 0x3E, 0x00, 0x07,
		0x00, 0x0B, 0x01, 0x02, 0x0F, 0x09, 0x18, 0x1E, 0x9F, 0xDF, 0x79, 0x78,
		0x18, 0x1C, 0x0C, 0x0C, 0x06, 0x8F, 0x87, 0x7D, 0x7F, 0x1C, 0x1F, 0x0B,
		0x01, 0x02, 0x0F, 0x0A, 0x78, 0x00, 0xFF, 0xB0, 0x61, 0x78, 0x20, 0x1C,
		0x10, 0x1E, 0x88, 0x0D, 0x66, 0xBC, 0x1F, 0xFC, 0x07, 0x30, 0x00, 0x0D,
		0x01, 0x02, 0x0F, 0x0B, 0xF0, 0x07, 0xFC, 0x0F, 0x03, 0xCE, 0x00, 0x26,
		0x00, 0x1E, 0x00, 0x0F, 0x80, 0x07, 0xC0, 0x03, 0xE0, 0x01, 0xF0, 0x00,
		0x18, 0x0B, 0x02, 0x02, 0x0F, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0x30, 0x60,
		0x18, 0x30, 0x0C, 0x18, 0x06, 0x0C, 0x03, 0x06, 0x00, 0x07, 0x01, 0x02,
		0x06, 0x05, 0x5E, 0x18, 0x86, 0x1E
	};
	
	// Usage: Copy the 3 commented lines to your code, removing the comments.
	//#include "XFont.h"
	//XFont xFont;
	//#include "MyriadPro-Regular_22_1bRt.h"
	
	DataStream_P	dataStream(glyphData, sizeof(glyphData));
	XFontR1BitDataStream xFontDataStream(&xFont, &dataStream);
	XFont::Font font(&fontHeader, charcodeRun, glyphDataOffset, &xFontDataStream);
	
	// Usage: The display needs to be set before using xFont.  This only needs
	// to be done once at the beginning of the program.
	// Use xFont.SetDisplay(&display, &MyriadPro_Regular_22_1bRt::font); to do this.
	// To change to this font anywhere after setting the display,
	// use: xFont.SetFont(&MyriadPro_Regular_22_1bRt::font);
}

#endif // MyriadPro_Regular_22_1bRt_h
