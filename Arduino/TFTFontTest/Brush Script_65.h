// Subset font created by SubsetFontCreator
// For subset: "FFllyy"

#ifndef Brush_Script_65_h
#define Brush_Script_65_h

#include "XFontGlyph.h"
#include "XFont16BitDataStream.h"

namespace Brush_Script_65
{
	const FontHeader	fontHeader PROGMEM =
	{
		1,		// version, currently version = 1
		0,		// oneBit, 1 = 1 bit per pixel, 0 = 8 bit (antialiased)
		0,		// rotated, glyph data is rotated (applies to 1 bit only)
		0,		// wideOffsets, 1 = 32 bit, 0 = 16 bit glyph data offsets
		0,		// monospaced, fixed width font (for this subset)
		58,		// ascent, font in pixels
		-22,	// descent, font in pixels
		80,		// height, font height (ascent+descent+leading) in pixels
		41,		// width, widest glyph advanceX within subset in pixels
		4,		// numCharcodeRuns
		3		// numCharCodes
	};
	
	const CharcodeRun	charcodeRun[] PROGMEM = // {start, entryIndex}, ...
	{
		{0x0046, 0}, {0x006C, 1}, {0x0079, 2}, {0xFFFF, 3}
	};
	
	const uint16_t	glyphDataOffset[] PROGMEM =
	{
		0x0000, 0x0284, 0x0453, 0x0698
	};
	
	const uint8_t	glyphData[] PROGMEM =
	{
		0x25, 0x00, 0x11, 0x33, 0x29, 0x1B, 0x00, 0xF5, 0x02, 0x1B, 0x2D, 0x39,
		0x44, 0x4F, 0x5A, 0x5A, 0x4B, 0x2A, 0x02, 0x17, 0x00, 0xF8, 0x0A, 0x31,
		0x5A, 0x81, 0xA0, 0xBF, 0xDE, 0xFA, 0x09, 0xFF, 0xFD, 0xEF, 0x93, 0x10,
		0x10, 0x00, 0xFA, 0x17, 0x4A, 0x7D, 0xAC, 0xDA, 0xFD, 0x12, 0xFF, 0xFF,
		0xC2, 0x0B, 0x00, 0xFB, 0x02, 0x2F, 0x6D, 0xA7, 0xE0, 0x19, 0xFF, 0xFF,
		0x2F, 0x07, 0x00, 0xFC, 0x16, 0x66, 0xAF, 0xEE, 0x1D, 0xFF, 0xFF, 0x55,
		0x05, 0x00, 0xFD, 0x06, 0x85, 0xF7, 0x20, 0xFF, 0xFF, 0x49, 0x04, 0x00,
		0xFE, 0x13, 0xC9, 0x21, 0xFF, 0xFE, 0xFC, 0x12, 0x03, 0x00, 0xFE, 0x09,
		0xCD, 0x22, 0xFF, 0xFF, 0xBE, 0x04, 0x00, 0xFF, 0x9A, 0x15, 0xFF, 0xF7,
		0xFC, 0xE0, 0xBF, 0x9E, 0x80, 0x63, 0x56, 0x6F, 0xC5, 0x05, 0xFF, 0xFF,
		0x5C, 0x03, 0x00, 0xFE, 0x3A, 0xFE, 0x0F, 0xFF, 0xF9, 0xEA, 0xBD, 0x98,
		0x72, 0x4C, 0x25, 0x05, 0x07, 0x00, 0xFE, 0x03, 0xCF, 0x03, 0xFF, 0xFA,
		0xEC, 0x08, 0x00, 0x00, 0x02, 0xD1, 0x0A, 0xFF, 0xFA, 0xFE, 0xDA, 0xA6,
		0x76, 0x48, 0x19, 0x0F, 0x00, 0xFF, 0xB0, 0x03, 0xFF, 0xFF, 0x7F, 0x03,
		0x00, 0xFF, 0x53, 0x08, 0xFF, 0xFC, 0xD7, 0x8E, 0x4C, 0x12, 0x13, 0x00,
		0xFA, 0x05, 0xED, 0xFF, 0xFF, 0xF4, 0x12, 0x03, 0x00, 0xFF, 0xC8, 0x05,
		0xFF, 0xFD, 0xDF, 0x7F, 0x27, 0x17, 0x00, 0xFF, 0x54, 0x03, 0xFF, 0xFF,
		0x89, 0x03, 0x00, 0xFF, 0x32, 0x04, 0xFF, 0xFB, 0xDF, 0x56, 0x02, 0x04,
		0x05, 0x17, 0x00, 0xFB, 0xC5, 0xFF, 0xFF, 0xF1, 0x12, 0x03, 0x00, 0xFF,
		0x8D, 0x04, 0xFF, 0xF9, 0xF2, 0xF5, 0xFE, 0xFF, 0xFF, 0xF5, 0x6F, 0x14,
		0x00, 0xFF, 0x43, 0x03, 0xFF, 0xFF, 0x7E, 0x04, 0x00, 0xFF, 0xD1, 0x05,
		0xFF, 0xFB, 0xFE, 0xD1, 0x91, 0x51, 0x12, 0x15, 0x00, 0xFB, 0xC3, 0xFF,
		0xFF, 0xE7, 0x0C, 0x04, 0x00, 0xF9, 0xB5, 0xFF, 0xFF, 0xFE, 0xC5, 0x6D,
		0x17, 0x18, 0x00, 0xFF, 0x50, 0x03, 0xFF, 0xFF, 0x61, 0x05, 0x00, 0xFC,
		0x1E, 0x98, 0x7C, 0x1F, 0x1A, 0x00, 0xFA, 0x05, 0xD9, 0xFF, 0xFF, 0xCC,
		0x01, 0x23, 0x00, 0xFB, 0x7A, 0xFF, 0xFF, 0xFE, 0x3A, 0x23, 0x00, 0xFB,
		0x22, 0xF6, 0xFF, 0xFF, 0x9A, 0x23, 0x00, 0xFA, 0x01, 0xBD, 0xFF, 0xFF,
		0xEA, 0x11, 0x23, 0x00, 0xFF, 0x6C, 0x03, 0xFF, 0xFF, 0x5C, 0x23, 0x00,
		0xFB, 0x24, 0xF5, 0xFF, 0xFF, 0xB5, 0x23, 0x00, 0xFA, 0x05, 0xCB, 0xFF,
		0xFF, 0xEF, 0x1B, 0x23, 0x00, 0xFF, 0x8B, 0x03, 0xFF, 0xFF, 0x60, 0x1B,
		0x00, 0xFB, 0x07, 0x3D, 0x75, 0x9E, 0xAB, 0x03, 0xAC, 0xFB, 0xAF, 0xFE,
		0xFF, 0xFF, 0xFA, 0x04, 0xAC, 0xFF, 0x53, 0x13, 0x00, 0xFB, 0x0A, 0x42,
		0x81, 0xC0, 0xF8, 0x0F, 0xFF, 0xFE, 0xDB, 0x0B, 0x10, 0x00, 0xFC, 0x3B,
		0x88, 0xC4, 0xFA, 0x12, 0xFF, 0xFE, 0xFC, 0x38, 0x10, 0x00, 0xFE, 0x48,
		0xFE, 0x15, 0xFF, 0xFF, 0x82, 0x10, 0x00, 0xFE, 0x01, 0xD3, 0x15, 0xFF,
		0xFE, 0xCB, 0x04, 0x10, 0x00, 0xFF, 0x49, 0x15, 0xFF, 0xFE, 0xF6, 0x28,
		0x11, 0x00, 0xFF, 0x7A, 0x09, 0xFF, 0xFD, 0xFE, 0xF4, 0xF0, 0x04, 0xFF,
		0xFA, 0xBB, 0x8C, 0x84, 0x7B, 0x71, 0x41, 0x12, 0x00, 0xF3, 0x07, 0x4F,
		0x60, 0x5B, 0x4F, 0x42, 0x35, 0x28, 0x1B, 0x0E, 0x02, 0x07, 0xC4, 0x03,
		0xFF, 0xFE, 0xC2, 0x06, 0x22, 0x00, 0xFF, 0xA3, 0x03, 0xFF, 0xFE, 0xE0,
		0x15, 0x22, 0x00, 0xFF, 0x7E, 0x03, 0xFF, 0xFE, 0xF3, 0x2E, 0x22, 0x00,
		0xFF, 0x59, 0x03, 0xFF, 0xFE, 0xFE, 0x50, 0x22, 0x00, 0xFE, 0x3C, 0xF8,
		0x03, 0xFF, 0xFF, 0x77, 0x22, 0x00, 0xFE, 0x26, 0xEC, 0x03, 0xFF, 0xFF,
		0x88, 0x22, 0x00, 0xFE, 0x17, 0xDD, 0x03, 0xFF, 0xFF, 0x95, 0x22, 0x00,
		0xFE, 0x0F, 0xCE, 0x03, 0xFF, 0xFE, 0xA1, 0x01, 0x1F, 0x00, 0xFC, 0x03,
		0x3A, 0x66, 0xC7, 0x03, 0xFF, 0xFE, 0xAC, 0x03, 0x1E, 0x00, 0xFD, 0x15,
		0x89, 0xEE, 0x05, 0xFF, 0xFE, 0xAD, 0x05, 0x1E, 0x00, 0xFE, 0x56, 0xEC,
		0x06, 0xFF, 0xFE, 0xA4, 0x03, 0x1E, 0x00, 0xFF, 0x83, 0x07, 0xFF, 0xFE,
		0x95, 0x02, 0x1E, 0x00, 0xFF, 0x91, 0x07, 0xFF, 0xFF, 0x7D, 0x1F, 0x00,
		0xFF, 0x76, 0x06, 0xFF, 0xFE, 0xF9, 0x5F, 0x1F, 0x00, 0xFE, 0x3A, 0xFC,
		0x05, 0xFF, 0xFE, 0xEA, 0x3C, 0x1F, 0x00, 0xFE, 0x05, 0xD6, 0x05, 0xFF,
		0xFE, 0xC6, 0x1C, 0x20, 0x00, 0xFF, 0x6C, 0x04, 0xFF, 0xFD, 0xFD, 0x8A,
		0x05, 0x21, 0x00, 0xFF, 0xC8, 0x03, 0xFF, 0xFE, 0xD5, 0x3C, 0x23, 0x00,
		0xFB, 0x6C, 0xF3, 0xD0, 0x67, 0x04, 0x22, 0x00, 0x0F, 0x00, 0x14, 0x27,
		0x13, 0x0F, 0x00, 0xFC, 0x1C, 0xA6, 0xF0, 0x51, 0x0E, 0x00, 0xFB, 0x52,
		0xF0, 0xFF, 0xFF, 0x98, 0x0D, 0x00, 0xFE, 0x6E, 0xFE, 0x03, 0xFF, 0xFF,
		0xA3, 0x0C, 0x00, 0xFF, 0x6C, 0x05, 0xFF, 0xFF, 0x91, 0x0B, 0x00, 0xFE,
		0x52, 0xFE, 0x05, 0xFF, 0xFF, 0x78, 0x0A, 0x00, 0xFE, 0x30, 0xF5, 0x06,
		0xFF, 0xFF, 0x48, 0x09, 0x00, 0xF6, 0x0F, 0xDC, 0xFF, 0xFF, 0xCF, 0x32,
		0xE2, 0xFF, 0xFA, 0x0B, 0x09, 0x00, 0xF7, 0xA8, 0xFF, 0xFF, 0xD7, 0x12,
		0x00, 0xE9, 0xFF, 0xBF, 0x09, 0x00, 0xF6, 0x5A, 0xFF, 0xFF, 0xF4, 0x29,
		0x00, 0x2D, 0xFF, 0xFF, 0x5E, 0x08, 0x00, 0xF5, 0x16, 0xED, 0xFF, 0xFF,
		0x6F, 0x00, 0x00, 0x8A, 0xFF, 0xEF, 0x09, 0x08, 0x00, 0xF6, 0xA8, 0xFF,
		0xFF, 0xCA, 0x02, 0x00, 0x0D, 0xEF, 0xFF, 0x8D, 0x08, 0x00, 0xFF, 0x45,
		0x03, 0xFF, 0xF9, 0x3C, 0x00, 0x00, 0x74, 0xFF, 0xF5, 0x17, 0x07, 0x00,
		0xF5, 0x04, 0xD5, 0xFF, 0xFF, 0xAE, 0x00, 0x00, 0x0F, 0xEC, 0xFF, 0x8A,
		0x08, 0x00, 0xFF, 0x6C, 0x03, 0xFF, 0xF9, 0x30, 0x00, 0x00, 0x86, 0xFF,
		0xEF, 0x14, 0x07, 0x00, 0xF5, 0x0B, 0xE9, 0xFF, 0xFF, 0xB4, 0x00, 0x00,
		0x25, 0xF8, 0xFF, 0x6A, 0x08, 0x00, 0xFF, 0x7B, 0x03, 0xFF, 0xF9, 0x43,
		0x00, 0x00, 0xBB, 0xFF, 0xCB, 0x02, 0x07, 0x00, 0xF5, 0x0B, 0xEC, 0xFF,
		0xFF, 0xDB, 0x01, 0x00, 0x62, 0xFF, 0xFC, 0x34, 0x08, 0x00, 0xFF, 0x72,
		0x03, 0xFF, 0xFA, 0x79, 0x00, 0x23, 0xF3, 0xFF, 0x80, 0x08, 0x00, 0xFE,
		0x03, 0xE1, 0x03, 0xFF, 0xFA, 0x24, 0x07, 0xC9, 0xFF, 0xC7, 0x03, 0x08,
		0x00, 0xFF, 0x51, 0x03, 0xFF, 0xFA, 0xD1, 0x00, 0x9D, 0xFF, 0xF2, 0x22,
		0x09, 0x00, 0xFF, 0xB9, 0x03, 0xFF, 0xFB, 0x8C, 0x75, 0xFF, 0xFE, 0x51,
		0x09, 0x00, 0xFE, 0x1A, 0xFD, 0x03, 0xFF, 0xFC, 0xB2, 0xFF, 0xFF, 0x80,
		0x0A, 0x00, 0xFF, 0x73, 0x06, 0xFF, 0xFE, 0xAF, 0x01, 0x0A, 0x00, 0xFF,
		0xC4, 0x05, 0xFF, 0xFE, 0xCD, 0x0C, 0x0A, 0x00, 0xFE, 0x13, 0xFD, 0x04,
		0xFF, 0xFE, 0xDB, 0x16, 0x0B, 0x00, 0xFF, 0x50, 0x04, 0xFF, 0xFE, 0xE7,
		0x22, 0x0C, 0x00, 0xFF, 0x8A, 0x04, 0xFF, 0xFF, 0x34, 0x0D, 0x00, 0xFF,
		0xB5, 0x03, 0xFF, 0xFF, 0xED, 0x0A, 0x00, 0xFF, 0x02, 0x03, 0x00, 0xFF,
		0xD7, 0x03, 0xFF, 0xFF, 0xCE, 0x09, 0x00, 0xFE, 0x25, 0x8B, 0x03, 0x00,
		0xFF, 0xE8, 0x03, 0xFF, 0xFF, 0xD0, 0x08, 0x00, 0xFD, 0x30, 0xE9, 0xAC,
		0x03, 0x00, 0xFF, 0xEB, 0x03, 0xFF, 0xFF, 0xEE, 0x07, 0x00, 0xFC, 0x3E,
		0xF1, 0xFF, 0x6B, 0x03, 0x00, 0xFF, 0xE1, 0x04, 0xFF, 0xFF, 0x3C, 0x05,
		0x00, 0xFC, 0x52, 0xF7, 0xFF, 0xA7, 0x04, 0x00, 0xFF, 0xCA, 0x04, 0xFF,
		0xF6, 0xBD, 0x03, 0x00, 0x00, 0x02, 0x7C, 0xFE, 0xFF, 0xCE, 0x0B, 0x04,
		0x00, 0xFF, 0xA3, 0x05, 0xFF, 0xF8, 0xC7, 0x66, 0x69, 0xCC, 0xFF, 0xFF,
		0xE3, 0x1A, 0x05, 0x00, 0xFF, 0x68, 0x0A, 0xFF, 0xFE, 0xEA, 0x2A, 0x06,
		0x00, 0xFE, 0x1C, 0xFC, 0x08, 0xFF, 0xFE, 0xE9, 0x2E, 0x08, 0x00, 0xFF,
		0xA7, 0x07, 0xFF, 0xFE, 0xDB, 0x24, 0x09, 0x00, 0xFE, 0x19, 0xE6, 0x05,
		0xFF, 0xFE, 0xA5, 0x0E, 0x0B, 0x00, 0xFA, 0x21, 0xAF, 0xF1, 0xEC, 0xAC,
		0x3D, 0x0B, 0x00, 0x18, 0x00, 0x23, 0x25, 0x1A, 0x09, 0x00, 0xFB, 0x18,
		0xA9, 0xEE, 0x89, 0x02, 0x14, 0x00, 0xFE, 0x37, 0xE7, 0x03, 0xFF, 0xFF,
		0x71, 0x07, 0x00, 0xFD, 0x0F, 0x97, 0x76, 0x09, 0x00, 0xFE, 0x4D, 0xF7,
		0x04, 0xFF, 0xFF, 0xC6, 0x06, 0x00, 0xFB, 0x08, 0xC5, 0xFF, 0xFF, 0x2E,
		0x07, 0x00, 0xFE, 0x50, 0xFA, 0x05, 0xFF, 0xFF, 0x93, 0x06, 0x00, 0xFF,
		0xA7, 0x03, 0xFF, 0xFF, 0x60, 0x06, 0x00, 0xFE, 0x40, 0xF9, 0x05, 0xFF,
		0xFE, 0xEE, 0x16, 0x05, 0x00, 0xFF, 0x7F, 0x04, 0xFF, 0xFF, 0x5D, 0x05,
		0x00, 0xFE, 0x2C, 0xF0, 0x05, 0xFF, 0xFE, 0xFE, 0x51, 0x05, 0x00, 0xFE,
		0x59, 0xFE, 0x03, 0xFF, 0xFE, 0xF7, 0x18, 0x04, 0x00, 0xFE, 0x10, 0xDE,
		0x06, 0xFF, 0xFF, 0x7F, 0x05, 0x00, 0xFE, 0x48, 0xFA, 0x04, 0xFF, 0xFF,
		0x8A, 0x04, 0x00, 0xFE, 0x01, 0xB1, 0x06, 0xFF, 0xFF, 0x9C, 0x05, 0x00,
		0xFE, 0x43, 0xF6, 0x04, 0xFF, 0xFE, 0xDE, 0x0B, 0x04, 0x00, 0xFF, 0x71,
		0x06, 0xFF, 0xFE, 0xA6, 0x03, 0x04, 0x00, 0xFE, 0x48, 0xF6, 0x04, 0xFF,
		0xFE, 0xFE, 0x42, 0x04, 0x00, 0xFE, 0x24, 0xF6, 0x05, 0xFF, 0xFE, 0xA2,
		0x03, 0x04, 0x00, 0xFE, 0x53, 0xF9, 0x05, 0xFF, 0xFF, 0x94, 0x05, 0x00,
		0xFF, 0xB9, 0x05, 0xFF, 0xFE, 0x91, 0x01, 0x04, 0x00, 0xFE, 0x6B, 0xFD,
		0x05, 0xFF, 0xFE, 0xDB, 0x0A, 0x04, 0x00, 0xFF, 0x3E, 0x04, 0xFF, 0xFE,
		0xFE, 0x76, 0x04, 0x00, 0xFE, 0x03, 0x8C, 0x06, 0xFF, 0xFE, 0xFE, 0x3E,
		0x05, 0x00, 0xFF, 0x97, 0x03, 0xFF, 0xFE, 0xFB, 0x55, 0x04, 0x00, 0xFE,
		0x12, 0xB9, 0x07, 0xFF, 0xFF, 0x95, 0x04, 0x00, 0xFD, 0x02, 0x00, 0xC8,
		0x03, 0xFF, 0xFF, 0xB4, 0x04, 0x00, 0xFE, 0x3C, 0xE2, 0x07, 0xFF, 0xFE,
		0xE1, 0x0D, 0x03, 0x00, 0xFC, 0x0F, 0xB6, 0x04, 0xDA, 0x03, 0xFF, 0xFA,
		0x5D, 0x00, 0x00, 0x08, 0x8A, 0xFC, 0x04, 0xFF, 0xFB, 0xCD, 0xFE, 0xFF,
		0xFF, 0x47, 0x03, 0x00, 0xFB, 0x24, 0xD8, 0xFF, 0x03, 0xCF, 0x03, 0xFF,
		0xFC, 0x2C, 0x01, 0x52, 0xDD, 0x05, 0xFF, 0xFB, 0x88, 0xB8, 0xFF, 0xFF,
		0x9F, 0x03, 0x00, 0xFA, 0x3B, 0xEC, 0xFF, 0x82, 0x00, 0xA2, 0x03, 0xFF,
		0xFE, 0xDF, 0xDD, 0x05, 0xFF, 0xF0, 0xFA, 0x61, 0x58, 0xFF, 0xFF, 0xE6,
		0x10, 0x00, 0x00, 0x4D, 0xF6, 0xFF, 0x96, 0x00, 0x00, 0x5C, 0x09, 0xFF,
		0xEE, 0xED, 0x3F, 0x0F, 0xE7, 0xFF, 0xFF, 0x57, 0x00, 0x00, 0x5C, 0xFB,
		0xFF, 0xA7, 0x02, 0x00, 0x00, 0x0D, 0xF2, 0x07, 0xFF, 0xF2, 0xDA, 0x25,
		0x00, 0x96, 0xFF, 0xFF, 0xBF, 0x00, 0x00, 0x69, 0xFE, 0xFF, 0xB8, 0x05,
		0x04, 0x00, 0xFF, 0x90, 0x06, 0xFF, 0xF2, 0xBD, 0x11, 0x00, 0x36, 0xFD,
		0xFF, 0xFB, 0x2B, 0x00, 0x73, 0xFF, 0xFF, 0xC8, 0x0A, 0x05, 0x00, 0xFE,
		0x17, 0xF3, 0x04, 0xFF, 0xF2, 0x93, 0x04, 0x00, 0x02, 0xCD, 0xFF, 0xFF,
		0x90, 0x00, 0x7B, 0xFF, 0xFF, 0xDC, 0x15, 0x07, 0x00, 0xFB, 0x5C, 0xFE,
		0xFF, 0xE4, 0x4F, 0x03, 0x00, 0xF6, 0x6B, 0xFF, 0xFF, 0xF0, 0x13, 0x85,
		0xFF, 0xFF, 0xEC, 0x25, 0x09, 0x00, 0xFD, 0x3F, 0x5C, 0x0A, 0x03, 0x00,
		0xF6, 0x16, 0xEF, 0xFF, 0xFF, 0x7A, 0x8E, 0xFF, 0xFF, 0xF7, 0x3A, 0x10,
		0x00, 0xF7, 0x9D, 0xFF, 0xFF, 0xE7, 0xA0, 0xFF, 0xFF, 0xFE, 0x53, 0x10,
		0x00, 0xFB, 0x35, 0xFE, 0xFF, 0xFF, 0xF2, 0x03, 0xFF, 0xFF, 0x78, 0x11,
		0x00, 0xFF, 0xC6, 0x06, 0xFF, 0xFF, 0xA0, 0x11, 0x00, 0xFF, 0x5A, 0x06,
		0xFF, 0xFE, 0xC5, 0x06, 0x10, 0x00, 0xFE, 0x07, 0xE0, 0x05, 0xFF, 0xFE,
		0xE2, 0x16, 0x11, 0x00, 0xFF, 0x74, 0x05, 0xFF, 0xFE, 0xF6, 0x32, 0x11,
		0x00, 0xFE, 0x0E, 0xEC, 0x05, 0xFF, 0xFF, 0x5E, 0x12, 0x00, 0xFF, 0x7D,
		0x05, 0xFF, 0xFF, 0x96, 0x12, 0x00, 0xFE, 0x0A, 0xEC, 0x04, 0xFF, 0xFE,
		0xC9, 0x06, 0x12, 0x00, 0xFF, 0x56, 0x04, 0xFF, 0xFE, 0xEB, 0x1D, 0x13,
		0x00, 0xFF, 0x70, 0x03, 0xFF, 0xFE, 0xFE, 0x48, 0x14, 0x00, 0xFF, 0x4E,
		0x03, 0xFF, 0xFF, 0x86, 0x15, 0x00, 0xFB, 0x0B, 0xF4, 0xFF, 0xC3, 0x04,
		0x16, 0x00, 0xFD, 0x76, 0xD4, 0x17, 0x12, 0x00
	};
	
	// Usage: Copy the 3 commented lines to your code, removing the comments.
	//#include "XFont.h"
	//XFont xFont;
	//#include "Brush Script_65.h"
	
	// Leave the next 3 lines here, as is.
	DataStream_P	dataStream(glyphData, sizeof(glyphData));
	XFont16BitDataStream xFontDataStream(&xFont, &dataStream);
	XFont::Font font(&fontHeader, charcodeRun, glyphDataOffset, &xFontDataStream);
	
	// The display needs to be set before using xFont.  This only needs
	// to be done once at the beginning of the program.
	// Use xFont.SetDisplay(&display, &Brush_Script_65::font); to do this.
	// To change to this font anywhere after setting the display,
	// use: xFont.SetFont(&Brush_Script_65::font);
}

#endif // Brush_Script_65_h

