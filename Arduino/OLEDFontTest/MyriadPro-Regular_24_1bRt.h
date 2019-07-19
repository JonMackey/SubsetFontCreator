// Subset font created by SubsetFontCreator
// For subset: "  -.0:AACCFFMMPPSS°°"

#ifndef MyriadPro_Regular_24_1bRt_h
#define MyriadPro_Regular_24_1bRt_h

#include "XFontGlyph.h"
#include "XFontR1BitDataStream.h"

namespace MyriadPro_Regular_24_1bRt
{
	const FontHeader	fontHeader PROGMEM =
	{
		1,		// version, currently version = 1
		1,		// oneBit, 1 = 1 bit per pixel, 0 = 8 bit (antialiased)
		1,		// rotated, glyph data is rotated (applies to 1 bit only)
		0,		// wideOffsets, 1 = 32 bit, 0 = 16 bit glyph data offsets
		0,		// monospaced, fixed width font (for this subset)
		18,		// ascent, font in pixels
		-6,		// descent, font in pixels
		29,		// height, font height (ascent+descent+leading) in pixels
		19,		// width, widest glyph advanceX within subset in pixels
		11,		// numCharcodeRuns
		21		// numCharCodes
	};
	
	const CharcodeRun	charcodeRun[] PROGMEM = // {start, entryIndex}, ...
	{
		{0x0020, 0}, {0x002D, 1}, {0x0030, 3}, {0x0041, 14}, {0x0043, 15},
		{0x0046, 16}, {0x004D, 17}, {0x0050, 18}, {0x0053, 19}, {0x00B0, 20},
		{0xFFFF, 21}
	};
	
	const uint16_t	glyphDataOffset[] PROGMEM =
	{
		0x0000, 0x0005, 0x000C, 0x0013, 0x002C, 0x003D, 0x0056, 0x006F,
		0x008C, 0x00A5, 0x00C0, 0x00D9, 0x00F2, 0x010B, 0x0115, 0x0136,
		0x0155, 0x016E, 0x0198, 0x01B3, 0x01CE, 0x01D8
	};
	
	const uint32_t	kGlyphDataLength = 472;
}

#endif // MyriadPro_Regular_24_1bRt_h

