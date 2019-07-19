// Subset font created by SubsetFontCreator
// For subset: "  --0:AACCFFMMPPSS°°"

#ifndef MyriadPro_Regular_36_1b_h
#define MyriadPro_Regular_36_1b_h

#include "XFontGlyph.h"
#include "XFont16BitDataStream.h"

namespace MyriadPro_Regular_36_1b
{
	const FontHeader	fontHeader PROGMEM =
	{
		1,		// version, currently version = 1
		1,		// oneBit, 1 = 1 bit per pixel, 0 = 8 bit (antialiased)
		0,		// rotated, glyph data is rotated (applies to 1 bit only)
		0,		// wideOffsets, 1 = 32 bit, 0 = 16 bit glyph data offsets
		0,		// monospaced, fixed width font (for this subset)
		27,		// ascent, font in pixels
		-9,		// descent, font in pixels
		43,		// height, font height (ascent+descent+leading) in pixels
		29,		// width, widest glyph advanceX within subset in pixels
		11,		// numCharcodeRuns
		20		// numCharCodes
	};
	
	const CharcodeRun	charcodeRun[] PROGMEM = // {start, entryIndex}, ...
	{
		{0x0020, 0}, {0x002D, 1}, {0x0030, 2}, {0x0041, 13}, {0x0043, 14},
		{0x0046, 15}, {0x004D, 16}, {0x0050, 17}, {0x0053, 18}, {0x00B0, 19},
		{0xFFFF, 20}
	};
	
	const uint16_t	glyphDataOffset[] PROGMEM =
	{
		0x0000, 0x0005, 0x000D, 0x0042, 0x005F, 0x0091, 0x00C0, 0x00F8,
		0x0127, 0x015C, 0x018E, 0x01C3, 0x01F8, 0x0206, 0x024A, 0x028B,
		0x02B9, 0x030D, 0x0341, 0x0372, 0x0384
	};
	
	const uint8_t	glyphData[] PROGMEM =
	{
		0x08, 0x00, 0x1B, 0x00, 0x00, 0x0B, 0x01, 0x10, 0x02, 0x09, 0xFF, 0xFF,
		0xC0, 0x12, 0x01, 0x03, 0x18, 0x10, 0x03, 0xE0, 0x0F, 0xF8, 0x1F, 0x7C,
		0x3C, 0x1C, 0x38, 0x0E, 0x78, 0x0E, 0x70, 0x0F, 0x70, 0x07, 0x70, 0x07,
		0xF0, 0x07, 0xF0, 0x07, 0xE0, 0x07, 0xE0, 0x07, 0xF0, 0x07, 0xF0, 0x07,
		0x70, 0x07, 0x70, 0x07, 0x70, 0x0F, 0x78, 0x0E, 0x38, 0x0E, 0x3C, 0x1C,
		0x1F, 0x7C, 0x0F, 0xF8, 0x07, 0xE0, 0x12, 0x04, 0x03, 0x18, 0x08, 0x0F,
		0x3F, 0xFF, 0xEF, 0xCF, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x12,
		0x02, 0x03, 0x18, 0x0F, 0x0F, 0xC0, 0xFF, 0xE3, 0xFF, 0xE3, 0x03, 0xC0,
		0x03, 0xC0, 0x03, 0x80, 0x07, 0x00, 0x0E, 0x00, 0x1C, 0x00, 0x78, 0x00,
		0xE0, 0x01, 0xC0, 0x07, 0x00, 0x1C, 0x00, 0x78, 0x01, 0xE0, 0x07, 0x80,
		0x1E, 0x00, 0x78, 0x01, 0xE0, 0x07, 0x80, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0x12, 0x02, 0x03, 0x18, 0x0E, 0x1F, 0xC1, 0xFF, 0x87, 0xFF, 0x10,
		0x1E, 0x00, 0x38, 0x00, 0xE0, 0x03, 0x80, 0x0E, 0x00, 0x70, 0x03, 0xC1,
		0xFC, 0x07, 0xF0, 0x03, 0xF0, 0x01, 0xE0, 0x03, 0xC0, 0x07, 0x00, 0x1C,
		0x00, 0x70, 0x01, 0xC0, 0x0F, 0x80, 0x7B, 0xFF, 0xCF, 0xFE, 0x0F, 0xE0,
		0x12, 0x01, 0x03, 0x18, 0x11, 0x00, 0x38, 0x00, 0x3C, 0x00, 0x3E, 0x00,
		0x1B, 0x00, 0x1F, 0x80, 0x1D, 0xC0, 0x0E, 0xE0, 0x0E, 0x70, 0x0E, 0x38,
		0x06, 0x1C, 0x07, 0x0E, 0x07, 0x07, 0x07, 0x03, 0x83, 0x81, 0xC3, 0x80,
		0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x1C, 0x00, 0x0E, 0x00, 0x07, 0x00,
		0x03, 0x80, 0x01, 0xC0, 0x00, 0xE0, 0x00, 0x70, 0x12, 0x02, 0x03, 0x18,
		0x0E, 0x3F, 0xFC, 0xFF, 0xF3, 0xFF, 0xCE, 0x00, 0x30, 0x00, 0xC0, 0x03,
		0x00, 0x1C, 0x00, 0x70, 0x01, 0xFF, 0x07, 0xFF, 0x00, 0x7E, 0x00, 0x78,
		0x00, 0xF0, 0x01, 0xC0, 0x07, 0x00, 0x1C, 0x00, 0x70, 0x03, 0xC0, 0x0E,
		0x80, 0x7B, 0xFF, 0xCF, 0xFE, 0x0F, 0xC0, 0x12, 0x01, 0x03, 0x18, 0x10,
		0x00, 0x3C, 0x01, 0xFC, 0x03, 0xFC, 0x0F, 0x80, 0x1E, 0x00, 0x1C, 0x00,
		0x38, 0x00, 0x38, 0x00, 0x71, 0xF0, 0x77, 0xFC, 0x7F, 0x7E, 0xF8, 0x1E,
		0xF0, 0x0F, 0xF0, 0x07, 0xE0, 0x07, 0xE0, 0x07, 0xF0, 0x07, 0x70, 0x07,
		0x70, 0x07, 0x78, 0x0E, 0x3C, 0x1E, 0x1F, 0x7C, 0x0F, 0xF8, 0x03, 0xE0,
		0x12, 0x02, 0x03, 0x18, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00,
		0xE0, 0x01, 0xC0, 0x07, 0x00, 0x0E, 0x00, 0x38, 0x00, 0x70, 0x01, 0xC0,
		0x03, 0x80, 0x0E, 0x00, 0x1C, 0x00, 0x70, 0x00, 0xE0, 0x03, 0xC0, 0x07,
		0x00, 0x1E, 0x00, 0x38, 0x00, 0xF0, 0x01, 0xC0, 0x03, 0x80, 0x0E, 0x00,
		0x1C, 0x00, 0x12, 0x01, 0x03, 0x18, 0x10, 0x03, 0xF0, 0x0F, 0xF8, 0x1E,
		0x7C, 0x38, 0x1E, 0x38, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x78, 0x0E, 0x38,
		0x1E, 0x3E, 0x3C, 0x1F, 0xF8, 0x07, 0xF0, 0x1F, 0xFC, 0x3C, 0x3E, 0x78,
		0x0E, 0x70, 0x0F, 0xF0, 0x07, 0xF0, 0x07, 0xF0, 0x07, 0x70, 0x0F, 0x78,
		0x0E, 0x3E, 0x3C, 0x1F, 0xF8, 0x07, 0xE0, 0x12, 0x01, 0x03, 0x18, 0x10,
		0x03, 0xE0, 0x0F, 0xF8, 0x1F, 0x7C, 0x38, 0x1E, 0x78, 0x0E, 0x70, 0x0F,
		0x70, 0x07, 0x70, 0x07, 0xF0, 0x07, 0x70, 0x07, 0x70, 0x07, 0x78, 0x0F,
		0x3C, 0x1F, 0x3F, 0xF7, 0x0F, 0xE7, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x1E,
		0x00, 0x1C, 0x00, 0x78, 0x00, 0xF0, 0x3F, 0xE0, 0x3F, 0xC0, 0x3E, 0x00,
		0x07, 0x02, 0x0A, 0x11, 0x04, 0x6F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0F,
		0xFF, 0x60, 0x16, 0x01, 0x02, 0x19, 0x14, 0x00, 0xF0, 0x00, 0x0F, 0x00,
		0x01, 0xF8, 0x00, 0x1F, 0x80, 0x01, 0xB8, 0x00, 0x39, 0xC0, 0x03, 0x9C,
		0x00, 0x39, 0xC0, 0x07, 0x0E, 0x00, 0x70, 0xE0, 0x07, 0x0E, 0x00, 0xE0,
		0x70, 0x0E, 0x07, 0x00, 0xE0, 0x70, 0x1C, 0x03, 0x81, 0xFF, 0xF8, 0x1F,
		0xFF, 0x83, 0x80, 0x3C, 0x38, 0x01, 0xC3, 0x80, 0x1C, 0x78, 0x01, 0xE7,
		0x00, 0x0E, 0x70, 0x00, 0xEF, 0x00, 0x0F, 0xE0, 0x00, 0x70, 0x15, 0x01,
		0x02, 0x19, 0x13, 0x00, 0x7F, 0x80, 0x3F, 0xFC, 0x1F, 0xFF, 0x07, 0xC0,
		0x01, 0xE0, 0x00, 0x78, 0x00, 0x0E, 0x00, 0x03, 0xC0, 0x00, 0x70, 0x00,
		0x0E, 0x00, 0x03, 0xC0, 0x00, 0x78, 0x00, 0x0F, 0x00, 0x01, 0xE0, 0x00,
		0x3C, 0x00, 0x03, 0x80, 0x00, 0x70, 0x00, 0x0F, 0x00, 0x01, 0xE0, 0x00,
		0x1E, 0x00, 0x01, 0xE0, 0x00, 0x3F, 0x00, 0x03, 0xFF, 0xF0, 0x1F, 0xFF,
		0x00, 0xFF, 0x80, 0x12, 0x03, 0x02, 0x19, 0x0D, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xC0, 0x0E, 0x00, 0x70, 0x03, 0x80, 0x1C, 0x00, 0xE0, 0x07, 0x00,
		0x38, 0x01, 0xFF, 0xEF, 0xFF, 0x7F, 0xFB, 0x80, 0x1C, 0x00, 0xE0, 0x07,
		0x00, 0x38, 0x01, 0xC0, 0x0E, 0x00, 0x70, 0x03, 0x80, 0x1C, 0x00, 0xE0,
		0x00, 0x1D, 0x02, 0x02, 0x19, 0x19, 0x3C, 0x00, 0x1E, 0x1E, 0x00, 0x0F,
		0x0F, 0x80, 0x0F, 0x87, 0xC0, 0x07, 0xE7, 0x60, 0x03, 0x73, 0xB8, 0x03,
		0xB9, 0xDC, 0x01, 0xDC, 0xEE, 0x00, 0xCE, 0x73, 0x80, 0xE7, 0x39, 0xC0,
		0x73, 0x9C, 0xE0, 0x31, 0xCE, 0x38, 0x38, 0xE7, 0x1C, 0x1C, 0x73, 0x8E,
		0x0C, 0x39, 0xC3, 0x8E, 0x1C, 0xE1, 0xC7, 0x0E, 0x70, 0xE3, 0x07, 0x38,
		0x3B, 0x83, 0x98, 0x1D, 0xC1, 0xDC, 0x0E, 0xC0, 0xFE, 0x03, 0x60, 0x3F,
		0x01, 0xF0, 0x1F, 0x80, 0xF0, 0x0F, 0xC0, 0x78, 0x07, 0xE0, 0x1C, 0x03,
		0x80, 0x13, 0x03, 0x02, 0x19, 0x0F, 0xFF, 0xC1, 0xFF, 0xE3, 0xFF, 0xE7,
		0x01, 0xEE, 0x01, 0xDC, 0x03, 0xF8, 0x07, 0xF0, 0x07, 0xE0, 0x1F, 0xC0,
		0x3B, 0x80, 0x77, 0x01, 0xCF, 0xFF, 0x9F, 0xFC, 0x3F, 0xE0, 0x70, 0x00,
		0xE0, 0x01, 0xC0, 0x03, 0x80, 0x07, 0x00, 0x0E, 0x00, 0x1C, 0x00, 0x38,
		0x00, 0x70, 0x00, 0xE0, 0x00, 0x12, 0x02, 0x02, 0x19, 0x0E, 0x07, 0xF0,
		0x7F, 0xE3, 0xFF, 0x9E, 0x00, 0xF0, 0x03, 0x80, 0x0E, 0x00, 0x38, 0x00,
		0xF0, 0x01, 0xE0, 0x07, 0xE0, 0x07, 0xE0, 0x0F, 0xE0, 0x07, 0xC0, 0x07,
		0x80, 0x0F, 0x00, 0x1C, 0x00, 0x70, 0x01, 0xC0, 0x07, 0x00, 0x3F, 0x01,
		0xEF, 0xFF, 0x3F, 0xF8, 0x3F, 0x80, 0x0B, 0x01, 0x02, 0x0A, 0x0A, 0x1E,
		0x1F, 0xC6, 0x1B, 0x06, 0xC0, 0xF0, 0x3C, 0x19, 0x8E, 0x3F, 0x03, 0x00
	};
	
	// Usage: Copy the 3 commented lines to your code, removing the comments.
	//#include "XFont.h"
	//XFont xFont;
	//#include "MyriadPro-Regular_36_1b.h"
	
	DataStream_P	dataStream(glyphData, sizeof(glyphData));
	XFont16BitDataStream xFontDataStream(&xFont, &dataStream);
	XFont::Font font(&fontHeader, charcodeRun, glyphDataOffset, &xFontDataStream);
	
	// Usage: The display needs to be set before using xFont.  This only needs
	// to be done once at the beginning of the program.
	// Use xFont.SetDisplay(&display, &MyriadPro_Regular_36_1b::font); to do this.
	// To change to this font anywhere after setting the display,
	// use: xFont.SetFont(&MyriadPro_Regular_36_1b::font);
}

#endif // MyriadPro_Regular_36_1b_h

