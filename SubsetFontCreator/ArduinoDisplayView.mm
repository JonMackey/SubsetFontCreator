//
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

	Please maintain this license information along with authorship
	and copyright notices in any redistribution of this code
*******************************************************************************/
/*
*	ArduinoDisplayView
*	
*	Created by Jon Mackey on 4/29/19.
*	Copyright © 2019 Jon. All rights reserved.
*/


#import "ArduinoDisplayView.h"
#include "SubsetFontCreator.h"
#include "XFont.h"
#include "XFont16BitDataStream.h"
#include "XFontR1BitDataStream.h"
#include "XFontRH1BitDataStream.h"
#include "BitmapDisplayController.h"

/*
*	When DEBUG_ARDUINO is defined the Arduino classes are used to populate the
*	native NSBitmapImageRep.  The Arduino classes support color and 1 bit
*	unrotated glyph data (regardless of what is selected in the GUI.)
*	The native code supports grayscale, 1 bit rotated and unrotated.
*
*	Note that the original non-arduino code is not up to date.
*/
#define DEBUG_ARDUINO	1

@implementation ArduinoDisplayView

/******************************** awakeFromNib ********************************/
- (void)awakeFromNib
{
	[super awakeFromNib];
	_displayPPMM = 1;
	_bitmapIsRotated = NO;
}

/********************************* setDisplay *********************************/
/*
*	Sets the Arduino display size for which the sample is being displayed.
*	Display		Res			Screen
*	Nokia 5110	84x48		29 x 19mm
*	1.3"		128x64		29.42 x 14.7mm
*	1.3"		240x240		23.4 x 23.4mm
*	0.96"		128x64		21.744 x 10.864mm
*	0.96"		160x80		21.696 x 10.8
*	0.91"		128x32		22.38 x 5.58mm
*/
- (void)setDisplay:(NSDictionary*)inDisplayDict
{
	_screenSize = NSSizeFromString([inDisplayDict objectForKey: @"screenSize"]);
	_deviceSize = NSSizeFromString([inDisplayDict objectForKey: @"deviceSize"]);
	_displayPPMM = _deviceSize.width/_screenSize.width;
	self.needsDisplay = YES;
}

/************************************ ppmm ************************************/
/*
*	Note that the calculated monitor ppi may be slightly different than the
*	ppi published ppi for the monitor.
*	The calculated value should be close enough.
*/
- (CGFloat)ppmm
{
	NSScreen*	screen = [[self window] screen];
	NSDictionary* devDesc = screen.deviceDescription;
	CGDirectDisplayID	displayID = [[devDesc objectForKey: @"NSScreenNumber"] unsignedIntValue];
	CGSize screenSize = CGDisplayScreenSize(displayID);
	NSSize	devSize = [[devDesc objectForKey: NSDeviceSize] sizeValue];
	return(devSize.width/screenSize.width);
}

#ifndef DEBUG_ARDUINO
/******************************* EntryIndexFor ********************************/
/*
*	Returns the glyph entry index for the passed charcode.
*/
uint16_t EntryIndexFor(
	uint16_t			inCharcode,
	uint16_t			inNumCharcodeRuns,
	const CharcodeRun*	inCharCodeRuns)
{
	uint16_t leftIndex = 0;
	{
		uint16_t current = 0;
		uint16_t rightIndex = inNumCharcodeRuns -1;
		while (leftIndex <= rightIndex)
		{
			current = (leftIndex + rightIndex) / 2;
			
			int	cmpResult = inCharCodeRuns[current].start - inCharcode;
			if (cmpResult == 0)
			{
				return(inCharCodeRuns[current].entryIndex);
			} else if (cmpResult > 0)
			{
				rightIndex = current - 1;
			} else
			{
				leftIndex = current + 1;
			}
		}
	}
	const CharcodeRun*	charcodeRun = &inCharCodeRuns[leftIndex-1];
	uint16_t	entryIndex = charcodeRun->entryIndex + inCharcode - charcodeRun->start;
	// Sanity check...
	// If the calculated entry index is less than the next run's entry index
	// then the calculated entry index is valid (else return index 0)
	// An invalid charcode does not have a corresponding glyph.
	return(charcodeRun[1].entryIndex > entryIndex ? entryIndex : 0);
}
#endif

/********************************* loadSample *********************************/
- (void)loadSample:(NSString*)inSampleText pointSize:(NSUInteger)inPointSize
			fontURL:(NSURL*)inFontURL
				options:(NSInteger)inOptions
					faceIndex:(NSInteger)inFaceIndex
						supplementalFontURL:(NSURL*)inSupplementalFontURL
							supplementalFaceIndex:(NSInteger)inSupplementalFaceIndex
								textColor:(NSColor*)inTextColor
									textBGColor:(NSColor*)inTextBGColor
										simulateMono:(BOOL)inSimulateMono
											log:(LogViewController*__nullable)inLog
{
	int createErr = -1;
	BOOL isDirectory = YES;
	if (inSampleText.length &&
		[[NSFileManager defaultManager] fileExistsAtPath:inFontURL.path isDirectory:&isDirectory] &&
		isDirectory == NO)
	{
		NSString*	xfntFilePath = [[[NSFileManager defaultManager] temporaryDirectory].path stringByAppendingPathComponent:@"arduinoDisplay.xfnt"];
		FILE* xfntFile = fopen(xfntFilePath.UTF8String, "w+");
		if (xfntFile)
		{
			SubsetCharcodeIterator	charcodeItr;
			charcodeItr.InitializeWithText(inSampleText.UTF8String);
			std::string	errorStr, warningStr, infoStr;
			BOOL	suppFontOK = [[NSFileManager defaultManager] fileExistsAtPath:inSupplementalFontURL.path isDirectory:&isDirectory] &&
										isDirectory == NO;
			if (suppFontOK == NO)
			{
				warningStr.append("Supplemental font not found.\n");
			}
			createErr = SubsetFontCreator::CreateXfntFile(inFontURL.path.UTF8String,
								xfntFile, xfntFile, (int32_t)inPointSize, (int)inOptions,
								charcodeItr, inFaceIndex,
								inSupplementalFontURL.path.UTF8String,
								inSupplementalFaceIndex,
								&errorStr, &warningStr, &infoStr);
			fclose(xfntFile);
		#ifndef DEBUG_ARDUINO
			_bitmapIsRotated = (inOptions & SubsetFontCreator::eRotated) != 0;
		#endif
			if (inLog)
			{
				if (errorStr.length())
				{
					[inLog postErrorString:[NSString stringWithUTF8String:errorStr.c_str()]];
				}
				if (warningStr.length())
				{
					[inLog postWarningString:[NSString stringWithUTF8String:warningStr.c_str()]];
				}
				if (infoStr.length())
				{
					[inLog postString:[NSString stringWithUTF8String:infoStr.c_str()]];
				}
				[inLog postString:[NSString stringWithFormat:@"565 text colors F/B = %04hX/%04hX",
					[ArduinoDisplayView to565Color:inTextColor], [ArduinoDisplayView to565Color:inTextBGColor]]];
				
			}
		}
		if (createErr == eSubsetNoErr)
		{
			NSData* xfntData = [NSData dataWithContentsOfFile:xfntFilePath];
			[self createBitmapImageRepForSample:inSampleText xfntData:xfntData
							textColor:inTextColor textBGColor:inTextBGColor
							simulateMono:inSimulateMono log:inLog];
		}
	}
	if (createErr)
	{
		_bitmapImageRep = nil;
	}
	self.needsDisplay = YES;
}

/****************************** CalcOneBitHeight ******************************/
/*
*	One bit fonts consume a whole 8 pixel high rows for each 8 bit row of data
*	when rendered on most rotated one pixel displays.
*	(A rotated display is a display that displays an 8 pixel high vertical
*	strip for each glyph data byte consumed.)
*	Simulate this by rounding up to the next even 8 pixel height.
*/
uint8_t CalcOneBitHeight(
	const FontHeader*	inFontHeader)
{
	return ((inFontHeader->rotated == 0 || inFontHeader->oneBit == 0 || (inFontHeader->height & 7) == 0) ?
				inFontHeader->height : (inFontHeader->height & 0xF8) + 8);
}

/********************************* to565Color *********************************/
+ (uint16_t)to565Color:(NSColor*)inColor
{
	CGFloat	r, g, b;
	NSColor*	color = [inColor colorUsingColorSpace:NSColorSpace.deviceRGBColorSpace];
	[color getRed:&r green:&g blue:&b alpha:nil];
	return((((uint16_t)(255 * b) & 0xF8) << 8) +
				(((uint16_t)(255 * g) & 0xFC) << 3) +
					(((uint16_t)(255 * r) & 0xF8) >> 3));
}

/*********************** createBitmapImageRepForSample ************************/
/*
*	Creates an NSBitmapImageRep of the sample text.
*	To make it easier to debug the Arduino classes, this function uses the
*	Arduino font and display classes where possible.
*/
- (void)createBitmapImageRepForSample:(NSString*)inSampleText xfntData:(NSData*)inXFntData
				textColor:(NSColor*)inTextColor textBGColor:(NSColor*)inTextBGColor
						simulateMono:(BOOL)inSimulateMono
							log:(LogViewController*__nullable)inLog
{
	const FontHeader*	fontHeader = (const FontHeader*)inXFntData.bytes;
	const CharcodeRun* charCodeRun = (const CharcodeRun*)&fontHeader[1];

#ifdef DEBUG_ARDUINO
	const uint16_t*	glyphDataOffset = (const uint16_t*)&charCodeRun[fontHeader->numCharcodeRuns];
	DataStream_S	glyphDataStream((const uint8_t*)&glyphDataOffset[fontHeader->numCharCodes +1],
										glyphDataOffset[fontHeader->numCharCodes]);
	XFont	xFont;
	XFontR1BitDataStream*	oneBitDataStream = NULL;
	XFontRH1BitDataStream*	oneBitHDataStream = NULL;
	DataStream*	dataStream = &glyphDataStream;
	/*
	*	When 1 bit rotated is used, insert a XFontR1BitDataStream to shift and
	*	reverse the data before it's passed on to XFont16BitDataStream.
	*	This is done to debug XFontR1BitDataStream and to sample rotated data.
	*/
	if (fontHeader->oneBit &&
		fontHeader->rotated)
	{
		if (fontHeader->horizontal)
		{
			oneBitHDataStream = new XFontRH1BitDataStream(&xFont, &glyphDataStream);
			dataStream = oneBitHDataStream;
		} else
		{
			oneBitDataStream = new XFontR1BitDataStream(&xFont, &glyphDataStream);
			dataStream = oneBitDataStream;
		}
	}
	XFont16BitDataStream xFontDataStream(&xFont, dataStream);
	XFont::Font	xFontFont(fontHeader, charCodeRun, glyphDataOffset, &xFontDataStream);
	xFont.SetTextColor([ArduinoDisplayView to565Color:inTextColor]);
	xFont.SetBGTextColor([ArduinoDisplayView to565Color:inTextBGColor]);
	// Because there is no display yet, set the font so that measurements can
	// be performed to determine the display dimensions.
	xFont.SetFont(&xFontFont);

	/*
	*	Determine the dimensions of the bitmap.
	*/
	uint8_t	monoWidth = 0;
	if (inSimulateMono)
	{
		SubsetCharcodeIterator	subsetItr;
		subsetItr.InitializeWithText(inSampleText.UTF8String);
		std::string subsetStr;
		monoWidth = xFont.WidestGlyph(subsetItr.GetSubset(subsetStr).c_str());
	}
	uint16_t	xHeight, xWidth;
	{
		uint16_t	lineWidths[255];
		uint8_t		numLineWidths = 255;
		xFont.MeasureStr(inSampleText.UTF8String, xHeight, xWidth, monoWidth, &numLineWidths, lineWidths);
		//xWidth = 160;
		[inLog postString:@"Line Widths:"];
		for (uint8_t lineIndex = 0; lineIndex < numLineWidths; lineIndex++)
		{
			[inLog postString:[NSString stringWithFormat:@" [%d] = %hdpx",
					(uint32_t)(lineIndex + 1), lineWidths[lineIndex]]];
		}
	}
	NSBitmapImageRep*	imageRep = [NSBitmapImageRep alloc];
	_bitmapImageRep = [imageRep initWithBitmapDataPlanes:NULL
							  pixelsWide:xWidth
							  pixelsHigh:xHeight
						   bitsPerSample:8
						 samplesPerPixel:4
								hasAlpha:YES
								isPlanar:NO
						  colorSpaceName:NSDeviceRGBColorSpace
							bitmapFormat:0
							 bytesPerRow:0
							bitsPerPixel:0];
	if (_bitmapImageRep)
	{
		uint8_t*	data = imageRep.bitmapData;
		BitmapDisplayController	display(xHeight, xWidth,
									(uint16_t)imageRep.bytesPerRow, data);
		// Enable special pixel mapping if 1 bit rotated horizontal.
		display.Set1BitRotatedHorizontal(oneBitHDataStream != NULL);
		// Set the display passing the font again to trigger the initialization
		// of XFont members related to the display depth.
		xFont.SetDisplay(&display, &xFontFont);
		xFont.DrawStr(inSampleText.UTF8String, true, monoWidth);
	}
	if (oneBitDataStream)
	{
		delete oneBitDataStream;
	}
	if (oneBitHDataStream)
	{
		delete oneBitHDataStream;
	}
#else
	// This case is not being maintained.
	const uint32_t*	glyphDataOffset = (const uint32_t*)&charCodeRun[fontHeader->numCharcodeRuns];
	const uint8_t*	glyphData = (const uint8_t*)&glyphDataOffset[fontHeader->numCharCodes +1];
	/*
	*	Iterate the sample text to determine the dimensions of the bitmap.
	*/
	NSUInteger	numChars = inSampleText.length;
	uint8_t		adjustedHeight = CalcOneBitHeight(fontHeader);	// Only different for 1 bit.
	NSInteger	height = adjustedHeight;
	NSInteger	rowWidth = 0;
	NSInteger	width = 0;
	NSUInteger	charIndex;
	Glyph*		glyph;
	uint16_t	charcode;
	for (charIndex = 0; charIndex < numChars; charIndex++)
	{
		charcode = [inSampleText characterAtIndex:charIndex];
		if (charcode >= 0x20)
		{
			uint16_t	entryIndex = EntryIndexFor(charcode, fontHeader->numCharcodeRuns, charCodeRun);
			glyph = (Glyph*)&glyphData[glyphDataOffset[entryIndex]];
			rowWidth += glyph->advanceX;
		} else
		{
			if (rowWidth > width)
			{
				width = rowWidth;
			}
			rowWidth = 0;
			height += adjustedHeight;
		}
	}
	if (rowWidth > width)
	{
		width = rowWidth;
	}

	if (fontHeader->rotated)
	{
		rowWidth = width;
		width = height;
		height = rowWidth;
	}

	NSBitmapImageRep*	imageRep = [NSBitmapImageRep alloc];
	_bitmapImageRep = [imageRep initWithBitmapDataPlanes:NULL
							  pixelsWide:width
							  pixelsHigh:height
						   bitsPerSample:8
						 samplesPerPixel:1
								hasAlpha:NO
								isPlanar:NO
						  colorSpaceName:NSDeviceWhiteColorSpace
							bitmapFormat:0
							 bytesPerRow:0
							bitsPerPixel:0];
	if (_bitmapImageRep)
	{
		uint8_t*	data = imageRep.bitmapData;
		// For rotated text the data is loaded right to left (which would be top to bottom on a rotated display)
		// For rotated, dataRowStart points to / starts at the end of the row.
		uint8_t*	dataRowStart = data + (fontHeader->rotated ? (width -1) : 0);	// of the current text row
		uint8_t*	dataGlyphStart = dataRowStart;	// of the current glyph within text row
		uint8_t*	dataGlyphRowStart;				// within the current glyph
		uint8_t*	dataGlyphPtr;
		size_t		bytesPerRow = imageRep.bytesPerRow;
		
		for (charIndex = 0; charIndex < numChars; charIndex++)
		{
			charcode = [inSampleText characterAtIndex:charIndex];
			if (charcode >= 0x20)
			{
				uint16_t	entryIndex = EntryIndexFor(charcode, fontHeader->numCharcodeRuns, charCodeRun);
				glyph = (Glyph*)&glyphData[glyphDataOffset[entryIndex]];
				const uint8_t*	dataPtr = ((const uint8_t*)glyph) + sizeof(Glyph);
				int8_t	glyphX = glyph->x;
				if (glyphX < 0)
				{
					glyphX = 0;	// Kerning not supported
				}
				if (fontHeader->rotated)
				{
					// For rotated, the data is loaded into the bitmap right to
					// left (which, because it's rotated, is top to bottom.)
					dataGlyphRowStart = dataGlyphStart + (glyphX * bytesPerRow) - glyph->y;
				} else
				{
					dataGlyphRowStart = dataGlyphStart + (glyph->y * bytesPerRow) + glyphX;
				}
				size_t	row = 0;
				size_t	column = 0;
				size_t	rows = glyph->rows;
				size_t	columns = glyph->columns;
				uint8_t*	dataGlyphColumnStart;
				if (fontHeader->oneBit)
				{
					if (fontHeader->rotated)
					{
						rows = (rows + 7)/8;
						for (; row < rows; row++)
						{
							dataGlyphColumnStart = dataGlyphRowStart;
							for (column = 0; column < columns; column++)
							{
								dataGlyphPtr = dataGlyphColumnStart;
								uint8_t	thisByte = *(dataPtr++);
								for (; thisByte; thisByte >>= 1)
								{
									*(dataGlyphPtr--) = (thisByte & 1) ? 0xFF:0;
								}
								dataGlyphColumnStart += bytesPerRow;
							}
							dataGlyphRowStart -= 8;
						}
					} else
					{
						columns = (columns + 7)/8;
						for (; row < rows; row++)
						{
							dataGlyphColumnStart = dataGlyphRowStart;
							for (column = 0; column < columns; column++)
							{
								dataGlyphPtr = dataGlyphColumnStart;
								uint8_t	thisByte = *(dataPtr++);
								for (; thisByte; thisByte <<= 1)
								{
									*(dataGlyphPtr++) = (thisByte & 0x80) ? 0xFF:0;
								}
								dataGlyphColumnStart += 8;
							}
							dataGlyphRowStart += bytesPerRow;
						}
					}
				} else
				{
					int8_t runLength = 0;
					uint8_t	runColor = 0;
					for (; row < rows; row++)
					{
						dataGlyphPtr = dataGlyphRowStart;
						uint8_t*	endOfRow = &dataGlyphPtr[columns];
						while (dataGlyphPtr != endOfRow)
						{
							if (runLength == 0)
							{
								runLength = (int8_t)*(dataPtr++);
								runColor = *(dataPtr++);
							}
							/*
							*	If the run length is negative THEN
							*	this is a run of unique values.
							*/
							if (runLength < 0)
							{
								while (dataGlyphPtr != endOfRow)
								{
									*(dataGlyphPtr++) = runColor;
									runLength++;
									if (runLength)
									{
										runColor = *(dataPtr++);
										continue;
									}
									break;
								}
								/*
								*	Else this is a run of same values.
								*/
								} else if (runLength > 0)
								{
									for (; dataGlyphPtr != endOfRow && runLength; runLength--)
									{
										*(dataGlyphPtr++) = runColor;
									}
								}
								/*
								*	If not at the end of the output buffer THEN
								*	load the next run length and its color
								*/
								if (dataGlyphPtr != endOfRow)
								{
									runLength = (int8_t)*(dataPtr++);
									runColor = *(dataPtr++);
								}
							}
							dataGlyphRowStart += bytesPerRow;
						}
#if 0
// Original code used when runs broke at the end of each row.  Runs now
// continue.  The data is assumed to be written into a defined window within
// the device/bitmap.
					for (; row < rows; row++)
					{
						dataGlyphPtr = dataGlyphRowStart;
						for (column = 0; column < columns;)
						{
							int32_t runLength = (int8_t)*(dataPtr++);
							if (runLength < 0)
							{
								runLength = -runLength;
								memcpy(dataGlyphPtr, dataPtr, runLength);
								dataPtr += runLength;
							} else
							{
								memset(dataGlyphPtr, *(dataPtr++), runLength);
							}
							dataGlyphPtr += runLength;
							column += runLength;
						}
						dataGlyphRowStart += bytesPerRow;
					}
#endif
				}
				if (fontHeader->rotated)
				{
					dataGlyphStart += (glyph->advanceX * bytesPerRow);
				} else
				{
					dataGlyphStart += glyph->advanceX;
				}
			} else
			{
				if (fontHeader->rotated)
				{
					dataRowStart -= adjustedHeight;
					dataGlyphStart = dataRowStart;
				} else
				{
					dataRowStart += (adjustedHeight * bytesPerRow);
					dataGlyphStart = dataRowStart;
				}
			}
		}
	}
#endif
}

/********************************** drawRect **********************************/
/*
*	Attempt to draw the actual physcal size of the display being simulated.  To
*	do this you need to know the native ppi (points per inch) of the monitor
*	that this view is currently being drawn on and the ppi of the display module
*	being simulated.
*
*	The monitor's ppi can be calculated programatically.
*/
- (void)drawRect:(NSRect)dirtyRect
{

    [super drawRect:dirtyRect];
	CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
	CGFloat	ppmmVal;
	NSRect	activeArea;
	CGFloat	inset = 0.5;
	if (_displayPPMM != 1)
	{
		ppmmVal = self.ppmm;
		activeArea = NSMakeRect(0,0,_screenSize.width * ppmmVal, _screenSize.height * ppmmVal);
	} else
	{
		ppmmVal = 1;	// Unscaled
		activeArea = [self bounds];
	}
	activeArea = NSInsetRect(activeArea, -inset, -inset);
	activeArea = NSOffsetRect(activeArea, inset, inset);
	NSRect clippedActiveArea = NSIntersectionRect([self bounds], activeArea);
	//CGContextSetGrayFillColor(context, 0.5, 1);
	//CGContextFillRect (context, clippedActiveArea);

	CGContextSetAllowsAntialiasing(context, false);
 	NSColor*	frameColor = [NSColor windowFrameColor];
 	CGContextSetLineWidth(context, 1.5);
 	CGContextSetStrokeColorWithColor(context, frameColor.CGColor);
	CGContextStrokeRect(context, clippedActiveArea);
	clippedActiveArea = NSInsetRect(clippedActiveArea, 1, 1);
	CGContextSetGrayFillColor(context, 0.5, 1);
	CGContextFillRect (context, clippedActiveArea);
	if (_bitmapImageRep)
	{
		CGContextClipToRect(context, clippedActiveArea);
		CGFloat	displayScale = ppmmVal/_displayPPMM;
		NSRect imageRect = clippedActiveArea;
		imageRect.size.height = _bitmapImageRep.pixelsHigh * displayScale;
		imageRect.size.width = _bitmapImageRep.pixelsWide * displayScale;
		if (_bitmapIsRotated)
		{
			NSAffineTransform*	rotateTransform = [[NSAffineTransform alloc] init];
			[rotateTransform rotateByDegrees:90];
			// We don't want any dithering, so round the translation to the nearest pixel...
			[rotateTransform translateXBy:ceil(clippedActiveArea.size.height - imageRect.size.width) yBy:floor(-imageRect.size.height-2)];
			[rotateTransform concat];
		} else
		{
			imageRect.origin.y = round(clippedActiveArea.size.height - imageRect.size.height +1);
		}

		[_bitmapImageRep drawInRect:imageRect];
	}
	CGContextSetAllowsAntialiasing(context, true);
}

@end
