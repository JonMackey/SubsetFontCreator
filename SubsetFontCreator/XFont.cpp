/*
*	XFont.cpp, Copyright Jonathan Mackey 2019-2023
*	Class for displaying subset bitmap fonts on select common displays.
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
#include "XFont.h"
#ifdef __MACH__
#include "pgmspace_stub.h"
#else
//#include <avr/pgmspace.h>
#include <Arduino.h>
#endif
#include <string.h>
#include "DataStream.h"
#include "DisplayController.h"
/*
*	The font header, charcode runs array, and glyph data offsets array are
*	assumed to be in near PROGMEM.  The Glyph data is accessed via a DataStream.
*	By using the DataStream abstraction, the data can be stored anywhere.
*
*	The xfnt specification allows for more than 64KB of glyph data.
*	This implementation only supports up to 64KB of glyph data (16 bit offsets.)
*/

const uint16_t	XFont::kEllipsisCharcode = 0x2026;

/*********************************** XFont ************************************/
XFont::XFont(void)
	: mDisplay(nullptr), mFontRows(0),
	  mHighlightEnabled(false), mFont(nullptr),
	  mTextColor(0xFFFF), mTextBGColor(0), mStartCol(0)
{
}

/******************************** SetDisplay **********************************/
void XFont::SetDisplay(
	DisplayController*	inDisplay,
	Font*				inFont)
{
	mDisplay = inDisplay;
	mFont = nullptr;	// Force a load
	SetFont(inFont);
}

/********************************** SetFont ***********************************/
void XFont::SetFont(
	Font*	inFont)
{
	if (inFont != mFont)
	{
		mFont = inFont;
		mCharcode = 0;
		if (inFont)
		{
			memcpy_P(&mFontHeader, mFont->header, sizeof(FontHeader));
			if (mDisplay)
			{
				mFontRows = (mFontHeader.rotated == 0 || (mFontHeader.height & 7) == 0) ?
											mFontHeader.height : (mFontHeader.height & 0xF8) + 8;
				if (mDisplay->BitsPerPixel() == 1)
				{
					mFontRows = (mFontHeader.height + 7)/8;
				}
			}
			mEllipsisWidth = LoadGlyph(kEllipsisCharcode) ? mGlyph.advanceX : 0;
		}
	}
}

/********************************* FindGlyph **********************************/
/*
*	Returns entryIndex within the glyphDataOffsets for inCharcode.
*	0xFFFF is returned if the glyph doesn't exist.
*/
uint16_t XFont::FindGlyph(
	uint16_t	inCharcode)
{
	uint16_t leftIndex = 0;
	const CharcodeRun*	charcodeRuns = mFont->charcodeRuns;
	const CharcodeRun*	charcodeRun = NULL;
	{
		uint16_t current = 0;
		uint16_t rightIndex = mFontHeader.numCharcodeRuns -1;
		while (leftIndex <= rightIndex)
		{
			current = (leftIndex + rightIndex) / 2;
			
			int32_t	cmpResult = (int32_t)pgm_read_word_near(&charcodeRuns[current].start) - inCharcode;
			if (cmpResult == 0)
			{
				charcodeRun = &charcodeRuns[current];
				break;
			} else if (cmpResult <= 0)
			{
				leftIndex = current + 1;
			} else if (current)
			{
				rightIndex = current - 1;
			} else
			{
				break;
			}
		}
	}
	uint16_t	entryIndex;
	if (!charcodeRun)
	{
		if (leftIndex)
		{
			charcodeRun = &charcodeRuns[leftIndex-1];
			entryIndex = pgm_read_word_near(&charcodeRun->entryIndex) +
							inCharcode - pgm_read_word_near(&charcodeRun->start);
			// Sanity check...
			// If the calculated entry index is less than the next run's entry index
			// then the calculated entry index is valid.
			// An invalid charcode does not have a corresponding glyph.
			if (entryIndex >= pgm_read_word_near(&charcodeRun[1].entryIndex))
			{
				entryIndex = 0xFFFF;
			}
		} else
		{
			entryIndex = 0xFFFF;
		}
	} else
	{
		entryIndex = pgm_read_word_near(&charcodeRun->entryIndex);
	}
	return(entryIndex);
}

/****************************** LoadGlyphHeader *******************************/
/*
*	Loads the glyph header of inEntryIndex into XFont->mGlyph.
*	Returns true if successful.
*/
bool XFont::LoadGlyphHeader(
	uint16_t	inEntryIndex)
{
	bool	success = false;
	if (inEntryIndex < mFontHeader.numCharCodes)
	{
		DataStream*	glyphData = mFont->glyphData;
		// At this point we have the entry index of the glyph within the GlyphDataOffsets
		// Load the glyph header
		success = glyphData->Seek(pgm_read_word_near(&mFont->glyphDataOffsets[inEntryIndex]), DataStream::eSeekSet);
		if (success)
		{
			glyphData->Read(sizeof(GlyphHeader), &mGlyph);
			if (mGlyph.x < 0)
			{
				mGlyph.x = 0; 	// Kerning not supported
			}
			// Forward kerning is not supported.
			// Increase advanceX enough so that the glyph isn't clipped.
			if (mGlyph.advanceX < (mGlyph.x + mGlyph.columns))
			{
				mGlyph.advanceX = mGlyph.x + mGlyph.columns;
			}
		}
	}
	return(success);
}

/********************************* LoadGlyph **********************************/
/*
*	Seeks the DataStream mGlyphData to point to the glyph data for inCharcode.
*	Returns true if the glyph was loaded.
*	mGlyph is initialized from the stream.
*/
bool XFont::LoadGlyph(
	uint16_t	inCharcode)
{
	bool	success = true;
	uint16_t	entryIndex = mCharcode != inCharcode ? FindGlyph(inCharcode) : mCharcodeIndex;
	/*
	*	Note that LoadGlyphHeader needs to be called before each DrawCharcode
	*	call because of a kluge in XFontDataStream::Read that uses the first
	*	read as a flag to initialize state variables.  If this wasn't the case
	*	then the loading could be skipped as an optimization. 
	*/
	if (entryIndex != 0xFFFF &&
		LoadGlyphHeader(entryIndex))
	{
		mCharcode = inCharcode;
		mCharcodeIndex = entryIndex;
	} else
	{
		success = false;
	}
	return(success);
}

/******************************** DrawCharcode ********************************/
/*
*	Draws a single glyph at the current display position.
*	The display column is advanced.
*	Do not pass control characters.
*	Do not set inFakeMonospaceWidth if the font is already monospace.
*/
bool XFont::DrawCharcode(
	uint16_t	inCharcode,
	uint8_t		inFakeMonospaceWidth)
{
	bool doContinue = LoadGlyph(inCharcode);
	while (doContinue)
	{
		bool	rotated = mFontHeader.rotated;
		bool	vertical = false;
		uint16_t	startRow = mDisplay->GetRow();
		uint8_t	rows = mGlyph.rows;
		uint8_t	columns = mGlyph.columns;
		if (inFakeMonospaceWidth)
		{
			mGlyph.x = (inFakeMonospaceWidth - columns)/2;
			mGlyph.advanceX = inFakeMonospaceWidth;
		}
		if (mFontHeader.oneBit)
		{
			vertical = rotated && !mFontHeader.horizontal;
			if (mDisplay->BitsPerPixel() == 1)
			{
				if (rotated)
				{
					rows = (rows + mGlyph.y + 7)/8;
				} else
				{
					columns = (columns + 7)/8;
				}
			} else if (rotated)
			{
		#ifdef __MACH__
				rows = ((rows + mGlyph.y + 7)/8) * 8;
		#else
				doContinue = false;
				break;	// rotated only supported by 1 bit displays
		#endif
			}
		}
		uint16_t	startColumn = mDisplay->GetColumn();
		uint16_t	rowsWritten = 0;
		/*
		*	Clear the pixels before the glyph...
		*/
		if (mGlyph.x)
		{
			mDisplay->FillBlock(mFontRows, mGlyph.x, mTextBGColor);
		}
		/*
		*	One bit rotated will have the y offset shifted into the data
		*	by the XFontR1BitDataStream or XFontRH1BitDataStream.
		*
		*	If it's not rotated AND
		*	the glyph needs to be shifted down THEN
		*	Clear the pixels above the glyph and adjust the top row.
		*/
		if (!rotated &&
			mGlyph.y &&
			columns)
		{
			mDisplay->FillBlock(mGlyph.y, columns, mTextBGColor);
			mDisplay->MoveTo(startRow + mGlyph.y, startColumn + mGlyph.x);
			rowsWritten = mGlyph.y;
		}
		if (vertical)
		{
			mDisplay->SetAddressingMode(DisplayController::eVertical);
		}
		doContinue = mDisplay->StreamCopyBlock(mFont->glyphData, rows, columns);
		if (vertical)
		{
			mDisplay->SetAddressingMode(DisplayController::eHorizontal);
		}
		if (doContinue)
		{
			rowsWritten += rows;
			/*
			*	Fill the pixels below the glyph with the BG color...
			*/
			if (columns &&
				rowsWritten < mFontRows)
			{
				uint16_t	savedColumn = mDisplay->GetColumn();
				mDisplay->MoveTo(startRow + rowsWritten, startColumn+mGlyph.x);
				mDisplay->FillBlock(mFontRows-rowsWritten, columns, mTextBGColor);
				mDisplay->MoveToColumn(savedColumn);
			}
			mDisplay->MoveToRow(startRow);
			doContinue = mDisplay->GetColumn() != 0;	// don't wrap
			/*
			*	Fill the pixels after the glyph (advance) with the BG color...
			*/
			if (doContinue)
			{
				if (mGlyph.advanceX > (mGlyph.x + columns))
				{
					mDisplay->FillBlock(mFontRows, mGlyph.advanceX - mGlyph.x - columns, mTextBGColor);
					doContinue = mDisplay->GetColumn() != 0;	// don't wrap
				}
				mDisplay->MoveToColumn(startColumn+mGlyph.advanceX);
			}
		}
		break;
	}
	return(doContinue);
}
	
//#include <stdio.h>
/********************************** DrawStr ***********************************/
/*
*	inUTF8Str is the string to be drawn starting at the current x,y position
*	of the display.  A glyph will only be drawn if it completely fits without
*	being truncated.  Drawing stops on the first truncation, the end of string,
*	or the character limit, whichever occurs first.
*	A character limit of value 0 is unlimited.
*/
void XFont::DrawStr(
	const char*	inUTF8Str,
	bool		inClearTillEOL,
	uint8_t		inFakeMonospaceWidth,
	uint8_t		inCharacterLimit)
{
	const char*	strPtr = inUTF8Str;
	uint16_t	startRow = mDisplay->GetRow();
	uint16_t	strStartColumn = mDisplay->GetColumn();
	mStartCol = strStartColumn;
	uint16_t	startColumn = strStartColumn;

	if (inFakeMonospaceWidth && mFontHeader.monospaced)
	{
		inFakeMonospaceWidth = 0;
	}
	uint8_t	charactersDrawn = 0;
	for (uint16_t charcode = NextChar(strPtr);
			charcode && (inCharacterLimit == 0 || charactersDrawn < inCharacterLimit);
				charcode = NextChar(strPtr), charactersDrawn++)
	{
		if (charcode >= ' ')
		{
			bool doContinue = DrawCharcode(charcode, inFakeMonospaceWidth);
			if (!doContinue)
			{
				doContinue = SkipToNextLine(strPtr);
			}
			if (doContinue)
			{
				continue;
			}
		} else if (charcode == '\n')
		{
			if (inClearTillEOL &&
				//startRow == mDisplay->GetRow() &&
				startColumn <= mDisplay->GetColumn())	// in case of wrap to 0
			{
				mDisplay->MoveToRow(startRow);
				EraseTillEndOfLine();
			}
			if (AdvanceRow(1, strStartColumn))
			{
				startRow = mDisplay->GetRow();
				startColumn = 0;	// if empty line & inClearTillEOL
				continue;
			}
		} else
		{
			// Ignore unsupported control characters
			continue;
		}
		break;
	}
	if (inClearTillEOL &&
		//startRow == mDisplay->GetRow() &&
		mDisplay->GetColumn() &&
		startColumn <= mDisplay->GetColumn())	// in case of wrap to 0
	{
		mDisplay->MoveToRow(startRow);
		EraseTillEndOfLine();
	}
}

/***************************** EraseTillEndOfLine *****************************/
void XFont::EraseTillEndOfLine(void)
{
	mDisplay->FillTillEndColumn(mFontRows, mTextBGColor);
}

/****************************** EraseTillColumn *******************************/
/*
*	Erases the area from the current column (which is generally the end column
*	of the last call to DrawStr), till inColumn.
*/
void XFont::EraseTillColumn(
	uint16_t	inColumn)
{
	uint16_t column = mDisplay->GetColumn();
	if (column < inColumn)
	{
		// FillBlock will clip the requested colunms if wider than the display.
		mDisplay->FillBlock(mFontRows, inColumn-column, mTextBGColor);
	}
}

/****************************** EraseFromColumn *******************************/
/*
*	Erase from the inColumn to the starting column of the last call to DrawStr().
*	This is used for centered and right justified strings to erase an area to
*	the left of the string.  For centered strings you may need to call both
*	EraseFromColumn and EraseTillColumn, calling EraseTillColumn first.
*/
void XFont::EraseFromColumn(
	uint16_t	inColumn)
{
	if (inColumn < mStartCol)
	{
		mDisplay->MoveToColumn(inColumn);
		mDisplay->FillBlock(mFontRows, mStartCol-inColumn, mTextBGColor);
	}
}

/******************************** DrawAligned *********************************/
/*
*	Note: This is an updated Draw routine that combines DrawStr,
*	DrawRightJustified, and DrawCentered, with the option of inserting an
*	ellipsis.  This only supports strings without control characters.
*
*	inUTF8Str is the string to be drawn.
*	inTextAlignment determines the drawing alignment.
*	inUseEllipsis determines whether the text is truncated with or without an
*	ellipsis.
*
*	Not supported by 1 bit displays.
*/
void XFont::DrawAligned(
	const char*				inUTF8Str,
	int32_t					inX,
	int32_t					inY,
	int32_t					inWidth,
	XFont::ETextAlignment	inAlignment,
	bool					inEraseUnusedArea)
{
	mDisplay->ClipX(inX, inWidth);
	const char*	strPtr = inUTF8Str;
	
	/*
	*	Measure each character and stop when the width is exceeded or the end
	*	of the string, which ever occurs first.
	*/
	uint16_t	width = 0;
	uint16_t 	charcode = NextChar(strPtr);
	uint16_t	prevCharcode = 0;
	uint16_t	charCount = 0;
	uint16_t	ellipsisCharCount = 0;
	uint16_t	truncatedWidth = 0;
	bool		needsTruncation = false;
	for (; charcode; charcode = NextChar(strPtr), charCount++)
	{
		if (charcode >= ' ')
		{
			if (prevCharcode == charcode ||
				LoadGlyph(charcode))
			{
				prevCharcode = charcode;
				width += mGlyph.advanceX;
				if (width <= inWidth)
				{
					/*
					*	In case truncation may be needed, save the char count
					*	at which the ellipsis will fit.
					*/
					if (!truncatedWidth &&
						(width + mEllipsisWidth) > inWidth)
					{
						ellipsisCharCount = charCount +1;
						truncatedWidth = width - mGlyph.advanceX + mEllipsisWidth;
					}
					continue;
				}
				/*
				*	At this point the string is too wide.
				*
				*	If this was the last charcode AND
				*	removing the width of the ellipsis allows the string to fit THEN
				*	exit the loop, the string doesn't need to be truncated.
				*/
				if (ellipsisCharCount)
				{
					charCount = ellipsisCharCount-1;
					width = truncatedWidth;
					needsTruncation = true;
				}
			}
		/*
		*	Else, charcode is not supported by this routine, exit loop
		*/
		}
		break;
	}
	int32_t	x = inX;
	if (inAlignment == eAlignRight)
	{
		x += (inWidth - width);
	} else if (inAlignment == eAlignCenter)
	{
		x += ((inWidth - width)/2);
	}
	mDisplay->MoveTo(inY, x);
	DrawStr(inUTF8Str, false, 0, charCount);
	if (needsTruncation &&
		inWidth > mEllipsisWidth)
	{
		DrawCharcode(kEllipsisCharcode);
	}
	if (inEraseUnusedArea)
	{
		if (inAlignment != eAlignRight)
		{
			EraseTillColumn(inX+inWidth);
		}
		if (inAlignment != eAlignLeft)
		{
			EraseFromColumn(inX);
		}
	}

}

/***************************** DrawRightJustified *****************************/
uint16_t XFont::DrawRightJustified(
	const char*	inUTF8Str,
	uint16_t	inRight,
	uint16_t	inWidth)
{
	if (inRight == 0 ||
		inRight > mDisplay->GetColumns())
	{
		inRight = mDisplay->GetColumns();
	}
	if (inWidth == 0)
	{
		uint16_t	unusedHeight;
		MeasureStr(inUTF8Str, unusedHeight, inWidth);
	}
	mDisplay->MoveToColumn(inRight > inWidth ? (inRight-inWidth) : 0);
	DrawStr(inUTF8Str);
	return(inWidth);
}

/******************************** DrawCentered ********************************/
uint16_t XFont::DrawCentered(
	const char*	inUTF8Str,
	uint16_t	inLeft,
	uint16_t	inRight,
	uint16_t	inWidth)
{
	if (inRight == 0 ||
		inRight > mDisplay->GetColumns())
	{
		inRight = mDisplay->GetColumns();
	}
	if (inWidth == 0)
	{
		uint16_t	unusedHeight;
		MeasureStr(inUTF8Str, unusedHeight, inWidth);
	}
	if (inLeft < inRight)
	{
		uint16_t	columns = inRight - inLeft;
		mDisplay->MoveToColumn(columns > inWidth ? (inLeft + ((columns-inWidth)/2)) : inLeft);
	}
	DrawStr(inUTF8Str);
	return(inWidth);
}

/******************************** WidestGlyph *********************************/
/*
*	Returns the widest glyph in the passed range string.  The format of the
*	range string is the same as you would use within SubsetFontCreator.
*	Returns zero if any of the chars within inUTF8RangeStr don't exist or the
*	format of inUTF8RangeStr is invalid.
*	Note: Only the first charcode of each range is tested to see if it exists.
*	The rest of the range is assumed to exist unless it's greater than the
*	number of charcodes in the subset font.
*/
uint8_t XFont::WidestGlyph(
	const char*	inUTF8RangeStr)
{
	uint8_t	widestGlyph = 0;
	const char*	strPtr = inUTF8RangeStr;
	for (uint16_t startChar = NextChar(strPtr); startChar;
								startChar = NextChar(strPtr))
	{
		uint16_t	endChar = NextChar(strPtr);
		if (startChar <= endChar)
		{
			uint16_t	entryIndex = FindGlyph(startChar);
			if (entryIndex != 0xFFFF)
			{
				uint16_t endEntryIndex = entryIndex + endChar - startChar;
				if (endEntryIndex < mFontHeader.numCharCodes)
				{
					for (; entryIndex <= endEntryIndex; entryIndex++)
					{
						if (LoadGlyphHeader(entryIndex))
						{
							if (mGlyph.advanceX > widestGlyph)
							{
								widestGlyph = mGlyph.advanceX;
							}
							continue;
						}
						break;
					}
					if (entryIndex > endEntryIndex)
					{
						continue;
					}
					// Fall through, charcode doesn't exist.
				}
			}
			// Fall through, charcode doesn't exist.
		}
		// Invalid range
		widestGlyph = 0;
		break;
	}
	return(widestGlyph);
}

/******************************* LoadFirstGlyph *******************************/
bool XFont::LoadFirstGlyph(
	const char*	inUTF8Str)
{
	uint16_t charcode = NextChar(inUTF8Str);
	return(charcode != 0 && LoadGlyph(charcode));
}

/********************************* MeasureStr *********************************/
bool XFont::MeasureStr(
	const char*	inUTF8Str,
	uint16_t&	outHeight,
	uint16_t&	outWidth,
	uint8_t		inFakeMonospaceWidth,
	uint8_t*	ioLineCount,
	uint16_t*	outLineWidths)
{
	const char*	strPtr = inUTF8Str;
	if (inFakeMonospaceWidth && mFontHeader.monospaced)
	{
		inFakeMonospaceWidth = 0;
	}
	bool	allGlyphsExist = true;
	/*
	*	One bit rotated consumes a whole 8 pixel high row for each 8 bit row
	*	of data when rendered on most rotated one pixel displays.
	*	(A rotated display is a display that displays an 8 pixel high vertical
	*	strip for each glyph data byte consumed.)
	*	Simulate this by rounding up to the next even 8 pixel height.
	*	This adjustedHeight is only different for rotated 1 bit.
	*/
	uint8_t adjustedHeight = (mFontHeader.rotated == 0 || (mFontHeader.height & 7) == 0) ?
										mFontHeader.height : (mFontHeader.height & 0xF8) + 8;

	outHeight = adjustedHeight;
	outWidth = 0;
	uint16_t	lineWidth = 0;
	uint8_t		lineWidthsSize = (ioLineCount && outLineWidths) ? *ioLineCount : 0;
	uint8_t		lineCount = 0;
	for (uint16_t charcode = NextChar(strPtr); charcode;
								charcode = NextChar(strPtr))
	{
		if (charcode >= ' ')
		{
			allGlyphsExist = LoadGlyph(charcode);
			if (allGlyphsExist)
			{
				if (inFakeMonospaceWidth)
				{
					lineWidth += inFakeMonospaceWidth;
				} else
				{
					lineWidth += mGlyph.advanceX;
				}
				continue;
			}
			break;
		} else if (charcode == '\n')
		{
			if (lineWidth > outWidth)
			{
				outWidth = lineWidth;
			}
			if (lineWidthsSize > lineCount)
			{
				outLineWidths[lineCount] = lineWidth;
			}
			lineCount++;
			lineWidth = 0;
			outHeight += adjustedHeight;
		}	// else ignore unsupported control characters
	}
	if (lineWidth > outWidth)
	{
		outWidth = lineWidth;
	}
	if (lineWidthsSize > lineCount)
	{
		outLineWidths[lineCount] = lineWidth;
	}
	lineCount++;
	if (ioLineCount)
	{
		*ioLineCount = lineCount;
	}
	return(allGlyphsExist);
}

/******************************* SkipToNextLine *******************************/
/*
*	Scans the string looking for a newline character.  If found returns true
*	and the past inUTF8Str points to the newline character.
*/
bool XFont::SkipToNextLine(
	const char*&	inUTF8Str)
{
	const uint8_t* strPtr = (uint8_t*)inUTF8Str;
	uint8_t thisChar;
	for (thisChar = *(strPtr++); thisChar != 0; thisChar = *(strPtr++))
	{
		if (thisChar != '\n')
		{
			continue;
		}
		strPtr--;
		inUTF8Str = (const char*)strPtr;
		break;
	}
	return(thisChar != 0);
}

/********************************** NextChar **********************************/
/*
*	Handles 1, 2, and 3 byte sequences (1 to 0xFFFF)
*	Returns the next charcode pointed to by inUTF8Str.
*	Advances inUTF8Str to point to the start of the next sequence.
*/
uint16_t XFont::NextChar(
	const char*&	inUTF8Str)
{
	uint16_t	nextChar = 0;
	const uint8_t* strPtr = (uint8_t*)inUTF8Str;
	uint16_t codepoint = *(strPtr++);
	if (codepoint)
	{
		if ((codepoint & 0x80) == 0)
		{
			nextChar = codepoint;
		// 2 byte sequence
		} else if ((codepoint & 0xE0) == 0xC0)
		{
			nextChar = (uint16_t)(((codepoint & 0x1F) << 6) +
							((uint16_t)(*(strPtr++)) & 0x3F));
		// 3 byte sequence
		} else if ((codepoint & 0xF0) == 0xE0)
		{
			nextChar = (uint16_t)(((codepoint & 0x0F) << 0xC) +
							(((uint16_t)(strPtr[0]) & 0x3F) << 6) +
								((uint16_t)(strPtr[1]) & 0x3F));
			strPtr+=2;
		}
		inUTF8Str = (const char*)strPtr;
	}
	return(nextChar);
}

/********************************* AdvanceRow *********************************/
/*
*	Relative move by N text rows and the absolute pixel column.
*	Returns true if there is space for the target row.
*/
bool XFont::AdvanceRow(
	uint16_t	inNumRows,
	uint16_t	inColumn) const
{
	uint16_t	top = (inNumRows * mFontRows) + mDisplay->GetRow();
	uint16_t	bottom = top + mFontRows -1;
	bool success = mDisplay->CanMoveTo(bottom, inColumn);
	if (success)
	{
		mDisplay->MoveTo(top, inColumn);
	}
	return(success);
}

/*********************************** MoveTo ***********************************/
/*
*	Absolute move to the Nth text row and the pixel column.
*	Will not move if there is no space for the target row.
*	Returns true if the move took place.
*/
bool XFont::MoveTo(
	uint16_t	inTextRow,	// 0 to N
	uint16_t	inColumn) const
{
	uint16_t	top = inTextRow * mFontRows;
	uint16_t	bottom = top + mFontRows -1;
	bool success = mDisplay->CanMoveTo(bottom, inColumn);
	if (success)
	{
		mDisplay->MoveTo(top, inColumn);
	}
	return(success);
}

/******************************** Calc565Color ********************************/
uint16_t XFont::Calc565Color(
	uint8_t		inTint)
{
	return(DisplayController::Calc565Color(mTextColor, mTextBGColor, inTint));
}

/****************************** XFontDataStream *******************************/
XFontDataStream::XFontDataStream(
	XFont*		inXFont,
	DataStream*	inSourceStream)
	: mXFont(inXFont), mSourceStream(inSourceStream)
{
}

/******************************** MakeCurrent *********************************/
XFont* XFont::Font::MakeCurrent(void)
{
	XFont* xFont = GetXFont();
	xFont->SetFont(this);
	return(xFont);
}
