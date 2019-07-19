#include "SubsetFontCreator.h"
#include <string>
#include <ft2build.h>
#include "XFontGlyph.h"
#include FT_FREETYPE_H


/******************************* Pixel24To16 ***********************************/
uint16_t Pixel24To16(
	uint32_t	inPixel24)
{
	return(((inPixel24 & 0xF80000) >> 8) + ((inPixel24 & 0xFC00) >> 5) + ((inPixel24 & 0xF8)>> 3));
}

/******************************* Pixel16To24 ***********************************/
uint32_t Pixel16To24(
	uint16_t	inPixel16)
{
	return(((inPixel16 & 0xF800) << 8) + ((inPixel16 & 0x7E0) << 5) + ((inPixel16 & 0x1F) << 3));
}

/******************************* ApplyTint24 ***********************************/
uint32_t ApplyTint24(
	uint32_t	inColor,
	uint32_t	inTint)
{
	return((((inColor & 0xFF0000) * inTint)/0xFF) & 0xFF0000) +
											((((inColor & 0xFF00) * inTint)/0xFF) & 0xFF00) +
												(((inColor & 0xFF) * inTint)/0xFF);
}

/******************************* ApplyTint16 ***********************************/
uint16_t ApplyTint16(
	uint32_t	inColor,
	uint32_t	inTint)
{
	return((uint16_t)((((inColor & 0xF800) * inTint)/0xFF) & 0xF800) +
											((((inColor & 0x7E0) * inTint)/0xFF) & 0x7E0) +
												(((inColor & 0x1F) * inTint)/0xFF));
}


/********************************** Next **************************************/
uint32_t SubsetCharcodeIterator::Next(void)
{
	if (mIsValid)
	{
		mCurrent++;
		mCharCodeIndex++;
		if (mCurrent > mEnd)
		{
			mPairIndex++;
			if (mPairIndex < mNumPairs)
			{
				mCurrent = [mSubset characterAtIndex:mPairIndex*2];
				mEnd = [mSubset characterAtIndex:(mPairIndex*2) + 1];
			} else
			{
				mIsValid = NO;
				mCurrent = 0x10000;
			}
		}
	}
	return(mCurrent);
}

/******************************* Initialize ***********************************/
void SubsetCharcodeIterator::Initialize(void)
{
	if (mSubset)
	{
		if ((mSubset.length & 1) == 0)
		{
			mIsValid = YES;
			mNumPairs = mSubset.length/2;
			unichar startChar, endChar;
			for (uint32_t pairIndex = 0; pairIndex < mNumPairs; pairIndex++)
			{
				startChar = [mSubset characterAtIndex:pairIndex*2];
				endChar = [mSubset characterAtIndex:(pairIndex*2) + 1];
				if (startChar <= endChar)
				{
					mNumCharCodes += (endChar - startChar + 1);
					continue;
				}
				[_logViewController postErrorString:[NSString stringWithFormat:@"Subset range %d is invalid, range start is greater than its end.", pairIndex]];
				mIsValid = NO;
				break;
			}
			/*
			*	If it's valid THEN
			*	load the first range.
			*/
			if (mIsValid)
			{
				mCurrent = [mSubset characterAtIndex:0];
				mEnd = [mSubset characterAtIndex:1];
			}
		} else
		{
			[_logViewController postErrorString:@"Subset ranges invalid, there must be an even number of characters in the subset string."];
			mIsValid = NO;
		}
	}
}

/************************** CreateRotatedBitmap ******************************/
/*
*	This is for the Nokia display where each byte of data represents an 8 bit
*	vertical strip.  inBitmap represents one byte as an 8 bit horizontal strip.
*	This routine converts the horizontal strips to vertical, with optional
*	padding bits at the top to account for centering (which would be harder to
*	do on the fly with vertical strips).
*	The rotated data is created in vertical strip order, one complete row of
*	8-bit bytes, then followed by successive rows, as needed.
*/
uint8_t* CreateRotatedBitmap(
	const uint8_t*	inBitmap,
	int				inRows,
	int				inWidth,	// Bits
	int				inPadding,	// Top padding, zero bits to position glyph on line
	bool			inBottomUp,	// Nokia is bottom up
	int&			outRows,
	int&			outCols)
{
	uint8_t*	rBitmap = NULL;
	if (inBitmap)
	{
		outCols = inWidth;
		if (inPadding < 0)
		{
			inPadding = 0;
		}
		outRows = (inPadding + inRows + 7)/8;
		size_t mapSize = outRows * outCols;
		rBitmap = new uint8_t[mapSize];
		memset(rBitmap, 0, mapSize);
		int vRowWidth = ((inWidth + 15)/16) * 2;;
		
		const uint8_t*	vBitMapPtr = inBitmap;
		uint8_t	vMask = 0x80;
		int vCol = 0;
		int hRow = inPadding/8;
		int vBitIndex = 0;
		uint8_t*	firstHRowPtr = rBitmap;
		if (hRow)
		{
			firstHRowPtr += (hRow * outCols);
			hRow = 0;
		}
		uint8_t*	hBitmapPtr = firstHRowPtr;
		inPadding = inPadding%8;
		while (true)
		{
			uint8_t	hMask = inBottomUp ? (1 << inPadding) : (0x80 >> inPadding);
			int vRow = 0;
			while (true)
			{
				if (*vBitMapPtr & vMask)
				{
					*hBitmapPtr |= hMask;
				}
				vRow++;
				if (vRow < inRows)
				{
					vBitMapPtr += vRowWidth;
					if (inBottomUp)
					{
						hMask <<= 1;
					} else
					{
						hMask >>= 1;
					}
					if (hMask == 0)
					{
						hMask = inBottomUp ? 1 : 0x80;
						hBitmapPtr += inWidth;
					}
				} else
				{
					break;
				}
			}
			vBitIndex++;
			if (vBitIndex < inWidth)
			{
				vMask >>= 1;
				if (vMask == 0)
				{
					vCol++;
					vMask = 0x80;
				}
				vBitMapPtr = &inBitmap[vCol];
				hRow++;
				hBitmapPtr = &firstHRowPtr[hRow];
			} else
			{
				break;
			}
		}
	} else
	{
		outCols = 0;
		outRows = 0;
	}
	return(rBitmap);
}

/******************************* createFile ***********************************/
/*
*	If inSubset is nil, then the complete font is dumped to the created file.
*	If inSubset is not nil, then select ranges of characters are dumped to the
*	created file.  The format is [start charcode, end charcode,...]
*	The ranges must be complete (have a start and an end), and they must
*	be valid (start charcode must be <= end charcode).
*/
- (void)createFile:(NSString*)inSubset
{
	[logTextView setString:@""];
	NSString*	filePath = pathComboBox.stringValue;
	NSString*	folderPath = folderPathComboBox.stringValue;
	int32_t	pointSize = pointSizeTextField.intValue;
	SubsetCharcodeIterator	charCodeIterator(inSubset);
	if (inSubset != nil && charCodeIterator.IsValid() == NO)
	{
		return;
	}
	bool	usingCCItr = charCodeIterator.IsValid();
	if (pointSize < 4)
	{
		pointSize = 4;
	} else if (pointSize > 72)
	{
		pointSize = 72;
	}
	bool	isMono = oneBitCheckBox.intValue != 0;
	bool	columnise = isMono && columniseBitsCheckBox.intValue != 0;
	std::string utf8FontPath([filePath UTF8String]);
	bool	folderPathIsDirectory = NO;
	if (folderPath &&
		[[NSFileManager defaultManager] fileExistsAtPath:folderPath isDirectory:&folderPathIsDirectory] &&
		folderPathIsDirectory)
	{
		NSString*	fnFormat = isMono ? (usingCCItr ? @"%@.%dbss.xfnt" : @"%@.%db.xfnt") : (usingCCItr ? @"%@.%dss.xfnt" : @"%@.%d.xfnt");
		NSString*	xfntPath = [folderPath stringByAppendingPathComponent:[NSString stringWithFormat:fnFormat, [filePath lastPathComponent], pointSize]];
		bool	xfntPathIsDirectory;
		if ([[NSFileManager defaultManager] fileExistsAtPath:xfntPath isDirectory:&xfntPathIsDirectory])
		{
			[_logViewController postWarningString:@"Deleting existing xfnt."];
		}
		std::string utf8XfntPath([xfntPath UTF8String]);
		if (utf8XfntPath.size())
		{
			FILE* file = fopen(utf8XfntPath.c_str(), "w+");
			if (file)
			{
				[_logViewController postInfoString:[NSString stringWithFormat:@"--------- Dumping \"%@\" to \"%@\" --------", [filePath lastPathComponent], xfntPath]];
				FT_Library ftLibrary = NULL;
				if (FT_Init_FreeType(&ftLibrary) == 0)
				{
					FT_Face	face = NULL;
					bool success = FT_New_Face(ftLibrary, utf8FontPath.c_str(), 0, &face) == 0;
					if (success)
					{
						
						FT_Error error = FT_Set_Char_Size(
									face,	/* handle to face object           */
									0,		/* char_width in 1/64th of points  */
									pointSize*64,	/* char_height in 1/64th of points */
									72,		/* horizontal device resolution    */
									72);	/* vertical device resolution      */
						
						FontHeader fontHeader;
						fontHeader.ascent = face->size->metrics.ascender;
						fontHeader.descent = face->size->metrics.descender;
						fontHeader.height = face->size->metrics.height;
						fontHeader.u.numCharCodes = charCodeIterator.GetNumCharCodes();	// Also sets maxCharCode to zero when not doing subsets
						FT_ULong	maxCharCode = 0;
						uint32_t	maxEntrySize = 0;
						int32_t ascent = (int32_t)(face->size->metrics.ascender >> 6);
						size_t entryMapSize = usingCCItr ? (charCodeIterator.GetNumCharCodes()+1)*4 : 0x10000*4;
						uint32_t	entryMap[0x10000];
						memset(entryMap, 0, entryMapSize);

						fwrite(&fontHeader, sizeof(FontHeader), 1, file);
						fseek(file, entryMapSize, SEEK_CUR);
						size_t	currFileOffset = entryMapSize + sizeof(FontHeader);

						if (error == 0)
						{
							GlyphHeader	glyphHdr;
							FT_ULong	charcode;
							FT_UInt		gindex;
							FT_Int32	loadFlags = FT_LOAD_RENDER;
							if (isMono)
							{
								loadFlags |= FT_LOAD_TARGET_MONO;
							}
							charcode = usingCCItr ? charCodeIterator.Current() : FT_Get_First_Char(face, &gindex);
							while (usingCCItr ? charCodeIterator.IsValid() : (gindex != 0))
							{
								if (charcode > 0 && charcode < 0xFFFF)
								{
									error = FT_Load_Char( face, charcode, loadFlags);
									if (error == 0)
									{
										if (maxCharCode < charcode)
										{
											maxCharCode = charcode;
										}
										FT_GlyphSlot  slot = face->glyph;
										FT_Bitmap& bitmap = slot->bitmap;
										int32_t	rows = bitmap.rows;
										int32_t width;;
										int32_t linePadding;
										uint8_t*	rotatedBitmap = NULL;
										unsigned char* bufferPtr = bitmap.buffer;
										if (isMono)
										{
											if (columnise)
											{
												int rotatedRows, rotatedColumns;
												int topY = (int)(face->size->metrics.ascender >> 6) - slot->bitmap_top;
												rotatedBitmap = CreateRotatedBitmap(bitmap.buffer, rows, bitmap.width, topY, true, rotatedRows, rotatedColumns);
												linePadding = 0;
												bufferPtr = rotatedBitmap;
												rows = rotatedRows;
												width = rotatedColumns;
											} else
											{
												width = (bitmap.width + 7)/8;
												linePadding = width & 1;
											}
										} else
										{
											width = bitmap.width;
											linePadding = bitmap.pitch - bitmap.width;
										}
										int32_t		runBufferMaxSize = ((width<<1)*rows) + 6;  // +6 allows for 2 bytes at the end + 4 byte alignment
										uint8_t*	runBuffer = new uint8_t[runBufferMaxSize];	// worst case size
										uint8_t*	runBufferPtr = runBuffer;
										glyphHdr.advanceX = slot->advance.x;
										glyphHdr.x = slot->bitmap_left;
										glyphHdr.y = ascent - slot->bitmap_top;
										glyphHdr.rows = rows;
										glyphHdr.columns = width;
										// Write the glyph header (but don't increment currFileOffset till after the runs are written)
										fwrite(&glyphHdr, sizeof(GlyphHeader), 1, file);
										
										int32_t col;
										uint32_t pixel;
										int32_t	runPixel;
										int32_t runLen;
										uint32_t bytesWritten = 0;
										if (rows)
										{
											for (int row = 0; row < rows; row++)
											{
												runPixel = *bufferPtr;
												runLen = 0;
												for (col = 0; col < width; col++)
												{
													pixel = *(bufferPtr++);
													if (isMono)
													{
														*(runBufferPtr++) = pixel;
														bytesWritten++;
													} else if (runPixel == pixel &&
														runLen < 255)
													{
														runLen++;
													} else
													{
														*(runBufferPtr++) = runLen;
														*(runBufferPtr++) = runPixel;
														bytesWritten++;
														runLen = 1;
														runPixel = pixel;
													}
												}
												if (!isMono)
												{
													if (runLen > 0)
													{
														*(runBufferPtr++) = runLen;
														*(runBufferPtr++) = runPixel;
														bytesWritten++;
													} else
													{
														[_logViewController postErrorString:@"Error, unexpected run end"];
													}
												}
												if (linePadding)
												{
													bufferPtr += linePadding;
												}
											}
										}
										if (!isMono)
										{
											*(runBufferPtr++) = 0;
											*(runBufferPtr++) = 0;
											bytesWritten++;
											bytesWritten <<= 1;
											// Align to 32 bit.
											if (bytesWritten & 2)
											{
												bytesWritten+=2;
												*(runBufferPtr++) = 0;
												*(runBufferPtr++) = 0;
											}
										}
										fwrite(runBuffer, bytesWritten, 1, file);
										if (rotatedBitmap)
										{
											delete [] rotatedBitmap;
											rotatedBitmap = NULL;
										}

										/*if (charcode == 'F')
										{
											fprintf(stderr, "currFileOffset = 0x%X\n", (uint32_t)currFileOffset);
										}*/
										entryMap[usingCCItr ? charCodeIterator.CharCodeIndex() : charcode] = (uint32_t)currFileOffset;
										currFileOffset += bytesWritten;
										currFileOffset += sizeof(GlyphHeader);
										{
											uint32_t thisEntrySize = bytesWritten + sizeof(GlyphHeader);
											if (thisEntrySize > maxEntrySize)
											{
												maxEntrySize = thisEntrySize;
											}
										}
										if (runBuffer)
										{
											delete [] runBuffer;
											runBuffer = NULL;
										}
									}
								}
								charcode = usingCCItr ? charCodeIterator.Next() : FT_Get_Next_Char(face, charcode, &gindex);
							}
						}
						if (!usingCCItr)
						{
							fontHeader.u.maxCharCode = maxCharCode;
						}
						fseek(file, 0, SEEK_SET);
						fwrite(&fontHeader, sizeof(FontHeader), 1, file);
						entryMap[usingCCItr ? charCodeIterator.GetNumCharCodes() : 0x10000-1] = currFileOffset;
						fwrite(entryMap, entryMapSize, 1, file);
						[[_logViewController appendFormat:@"entryMapSize = %d, FontHeaderSize = %d, maxCharCode = 0x%XU, currFileOffset = %d, maxEntrySize = %d\n",
							(uint32_t)entryMapSize, (uint32_t)sizeof(FontHeader), (uint32_t)maxCharCode, (uint32_t)currFileOffset, maxEntrySize] post];
					}
					FT_Done_FreeType(ftLibrary);
				}
				fclose(file);
			}
		}
	}
}

/******************************* doSample ***********************************/
- (IBAction)doSample:(id)inSender
{
	NSString*	filePath = pathComboBox.stringValue;
	int32_t	pointSize = pointSizeTextField.intValue;
	if (pointSize < 4)
	{
		pointSize = 4;
	} else if (pointSize > 72)
	{
		pointSize = 72;
	}
	std::string utf8FontPath([filePath UTF8String]);
	NSString*	sampleText = [charsTextField stringValue];
	//[[NSUserDefaults standardUserDefaults] stringForKey:@"sampleText"];
	[logTextView setString:@""];
	FT_Library ftLibrary = NULL;
	bool	isMono = oneBitCheckBox.intValue != 0;
	bool	columnise = isMono && columniseBitsCheckBox.intValue != 0;
	if (FT_Init_FreeType(&ftLibrary) == 0)
	{
		FT_Face	face = NULL;
		bool success = FT_New_Face(ftLibrary, utf8FontPath.c_str(), 0, &face) == 0;
		if (success)
		{
			int	pen_x, pen_y;//, n;
			pen_x = 300;
			pen_y = 200;
		//	http://www.freetype.org/freetype2/docs/tutorial/step1.html
			FT_Error error = FT_Set_Char_Size(
						face,	/* handle to face object           */
						0,		/* char_width in 1/64th of points  */
						pointSize*64,	/* char_height in 1/64th of points */
						72,		/* horizontal device resolution    */
						72);	/* vertical device resolution      */
			if (error == 0)
			{
				NSUInteger sampleLen = [sampleText length];
				for (NSUInteger charIndex = 0; charIndex < sampleLen; charIndex++)
				{
					FT_ULong charcode = [sampleText characterAtIndex:charIndex];
					FT_Int32  loadFlags = FT_LOAD_RENDER;
					if (isMono)
					{
						loadFlags |= FT_LOAD_TARGET_MONO;
					}
					error = FT_Load_Char( face, charcode, loadFlags);
					if (error == 0)
					{
					/*
					metrics	FT_Glyph_Metrics	

					metrics.width	FT_Pos	1472	<< same as advance
					metrics.height	FT_Pos	1728
					metrics.vertBearingY	FT_Pos	576
					metrics.vertAdvance	FT_Pos	2880
					*/
						FT_GlyphSlot  slot = face->glyph;
						FT_Bitmap& bitmap = slot->bitmap;
						char dumpBuf[0xFFFF];
						char* dumpBufPtr = dumpBuf;
						char *	endDumpBufPtr = &dumpBuf[0xFFFF];
						dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "rows = %d, width = %d, pitch = %d, num_grays = %hd, pixel_mode = %d\n\n",
							bitmap.rows, bitmap.width, bitmap.pitch, bitmap.num_grays, (int)bitmap.pixel_mode);
						int	numRows = bitmap.rows;
						uint8_t* bufferPtr = bitmap.buffer;
						int col;
						uint32_t pixel;//, invPixel;
						int linePadding;
						int	runPixel;
						int runLen;
						int bytesWritten = 0;
						int width;
						uint8_t*	rotatedBitmap = NULL;
						if (isMono)
						{
							if (columnise)
							{
								int rotatedRows, rotatedColumns;
								int topY = (int)(face->size->metrics.ascender >> 6) - slot->bitmap_top;
								rotatedBitmap = CreateRotatedBitmap(bitmap.buffer, numRows, bitmap.width, topY, true, rotatedRows, rotatedColumns);
								linePadding = 0;
								bufferPtr = rotatedBitmap;
								numRows = rotatedRows;
								width = rotatedColumns;
							} else
							{
								width = (bitmap.width + 7)/8;
								linePadding = width & 1;
							}
						} else
						{
							width = bitmap.width;
							linePadding = bitmap.pitch - bitmap.width;
						}
						if (columnise)
						{
							size_t	glyphDataSize = numRows * width;
							size_t	glyphSize = sizeof(GlyphHeader) + glyphDataSize;
							Glyph*	glyph = (Glyph*)(new uint8_t[glyphSize]);
							glyph->advanceX = slot->advance.x;
							glyph->x = slot->bitmap_left;
							glyph->y = (int32_t)(face->size->metrics.ascender >> 6) - slot->bitmap_top;
							glyph->rows = numRows;
							glyph->columns = width;
							memcpy(&glyph->data[0], rotatedBitmap, glyphDataSize);
							[nokiaView setGlyph:glyph];
							delete [] (uint8_t*)glyph;
						}
						if (numRows)
						{
							for (int row = 0; row < numRows; row++)
							{
								runPixel = *bufferPtr;
								runLen = 0;
								for (col = 0; col < width; col++)
								{
									pixel = *(bufferPtr++);
								#if 0
									pixel = 64;
									if (pixel > 20)
									{
										invPixel = 255 - pixel;
										
										uint32_t fbPx24 = 0xFFFFFF;
										uint32_t fbPx16 = Pixel24To16(fbPx24);
										uint32_t fb24 = Pixel16To24(fbPx16);
										uint32_t fbPxS24 = ApplyTint24(fb24, invPixel);
														
										uint32_t fntPx24 = 0xC0C0C0;
										uint32_t fntPxS24 = ApplyTint24(fntPx24, pixel);
										
										uint32_t c24 = fntPxS24 + fbPxS24;
										uint16_t c16 = Pixel24To16(c24);
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "pixel = %X -> invPixel = %X\n", pixel, invPixel);
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "fbPx24 = %X -> fbPx16 = %X -> fb24 = %X -> fbPxS24 = %X\n", fbPx24, fbPx16, fb24, fbPxS24);
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "fntPx24 = %X -> fntPxS24 = %X\n", fntPx24, fntPxS24);
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "c24 = %X -> c16 = %hX\n", c24, c16);
										break;
									}
								#endif
									#if 1
									#if 0
									if (pixel == 0)
									{
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\t");
									} else
									{
										if (col > colEnd)
										{
											colEnd = col;
										}
										if (col < colStart)
										{
											colStart = col;
										}
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\t%X", pixel);
									}
									#else
									if (isMono)
									{
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "%0.2X ", pixel);
										bytesWritten++;
									} else
									{
										if (runPixel == pixel &&
											runLen < 255)
										{
											runLen++;
										} else
										{
											dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "%d,%d,", runLen, runPixel);
											bytesWritten++;
											runLen = 1;
											runPixel = pixel;
										}
									}
									#endif
									#endif
								}
								if (isMono)
								{
									dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\n");
								} else
								{
									if (runLen > 0)
									{
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "%d,%d,\n", runLen, runPixel);
										bytesWritten++;
									} else
									{
										dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "Error\n");
									}
								}
								if (linePadding)
								{
									bufferPtr += linePadding;
								}
							}
						}
						dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\n");
						for (int k = slot->advance.x >> 6; k >= 0; k--)
						{
							dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\t..");
						}
						if (!isMono)
						{
							bytesWritten <<= 1;
						}
						dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\n\ncharcode = U%0.4X, bytesWritten = %d, advance.x = %ld, vertAdvance = %ld, bitmap_left = %d, bitmap_top = %d\n",
							(int)charcode, bytesWritten, slot->advance.x >> 6, slot->metrics.vertAdvance >> 6, slot->bitmap_left, slot->bitmap_top);
						{
							int ascent = (int)(face->size->metrics.ascender >> 6);
							int descent = (int)(face->size->metrics.descender >> 6);
							int lineHeight = (int)( face->size->metrics.height >> 6);
							int topY = ascent - slot->bitmap_top;
							dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "ascent = %d, descent = %d, lineHeight = %d, topY = %d\n",
								ascent, descent, lineHeight, topY);
	/*dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\tface->size.metrics.ascender = %ld\n", face->size->metrics.ascender >> 6);
	dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\tface->size.metrics.descender = %ld\n", face->size->metrics.descender >> 6);
	dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\tface->size->metrics.ascender - face->size->metrics.descender = %ld\n", (face->size->metrics.ascender - face->size->metrics.descender) >> 6);
	dumpBufPtr += snprintf(dumpBufPtr, endDumpBufPtr-dumpBufPtr, "\tface->size->metrics.height = %ld\n", face->size->metrics.height >> 6);*/
						}
						if (rotatedBitmap)
						{
							delete [] rotatedBitmap;
						}
						[[_logViewController appendUTF8String:dumpBuf] post];
						
						pen_x += slot->advance.x >> 6;
					}
				}
			}
			
			/*
*/
		}
		FT_Done_FreeType(ftLibrary);
	}
	
}

@end

