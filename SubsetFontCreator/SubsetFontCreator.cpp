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
*	SubsetFontCreator
*	
*	Created by Jon Mackey on 12/14/18.
*	Copyright © 2020 Jon Mackey. All rights reserved.
*/


#include "SubsetFontCreator.h"
#include "TabbedFileStream.h"
#include <string>
#include <ft2build.h>

#include FT_FREETYPE_H
/********************************* DumpBinary *********************************/
void DumpBinaryX(
	uint8_t		inValue)
{
	uint8_t	nMask = 0x80;
	char	buffer[50];
	char*	buffPtr = buffer;
	for (; nMask != 0; nMask >>= 1)
	{
		*(buffPtr++) = (inValue & nMask) ? '*':'.';
	}
	*buffPtr = 0;
	fprintf(stderr, "%s ", buffer);
}

/***************************** CreateRotatedData ******************************/
/*
*	This is for displays where each byte of data represents 8 pixels vertically.
*	inBitmap represents one 8 bit byte as 8 pixels horizontally.
*	This routine converts the horizontal bits to vertical (i.e. rotated.)
*	The data returned is packed, meaning there are no breaks for columns or
*	rows, it's a continuous string of bits. Packing saves space and doesn't cost
*	much to unpack.
*
*	SSD1306 (OLED) and PCD8544 (Nokia 5110) use MSB bottom.
*
*	inHorizontal controls whether the rotated and packed data is stored
*	as horizontal or vertical strips.	Horizontal: 123		Vertical:	147
*													456					258
*													789					369
*
*	The vertical case is pretty straight forward, just a serailly rotated block.
*	The horizontal case needs to account for the glyph's y offset.
*/
uint8_t* SubsetFontCreator::CreateRotatedData(
	const uint8_t*	inBitmap,
	int				inRows,		// bits
	int				inColumns,	// bits
	int				inYOffset,	// only used by the horizontal case.
	bool			inMSBTop,
	bool			inHorizontal,
	size_t&			outDataLen)
{
	uint8_t*	rotatedData = NULL;
	outDataLen = ((inRows * inColumns) + 7)/8;
	if (inBitmap &&
		outDataLen > 0)
	{
		rotatedData = new uint8_t[outDataLen];
		// inBitmap is 16 bit word aligned
		int bytesPerRow = ((inColumns + 15)/16) * 2;
		
		const uint8_t*	firstRowPtr;
		const uint8_t*	bitMapPtr;
		uint8_t		byteOut = 0;
		uint8_t		maskIn;
		uint8_t		maskOutInit = inMSBTop ? 0x80 : 1;
		uint8_t		maskOut = maskOutInit;
		uint8_t*	dataOutPtr = rotatedData;
		int			endRow = 0;

		if (inYOffset < 0)
		{
			inYOffset = 0;	// Kerning not supported
		}
		/*
		*	This outer loop only loops once for vertical.
		*	For horizontal it loops (int)((inRows + 7)/8) times.
		*/
		for (int startRow = 0; startRow < inRows; startRow = endRow)
		{
			if (inHorizontal)
			{
				if (startRow)
				{
					endRow = startRow+8;
				} else
				{
					endRow = 8 - (inYOffset % 8);
				}
				if (endRow > inRows)
				{
					endRow = inRows;
				}
				firstRowPtr = &inBitmap[startRow*bytesPerRow];
			} else
			{
				endRow = inRows;
				firstRowPtr = inBitmap;
			}
			maskIn = 0x80;
			for (int column = 0; column < inColumns; column++)
			{
				bitMapPtr = firstRowPtr;
				for (int row = startRow; row < endRow; row++)
				{
					if (*bitMapPtr & maskIn)
					{
						byteOut |= maskOut;
					}
					maskOut = inMSBTop ? (maskOut >>= 1) : (maskOut <<= 1);
					if (maskOut == 0)
					{
						maskOut = maskOutInit;
						*(dataOutPtr++) = byteOut;
						byteOut = 0;
					}
					bitMapPtr += bytesPerRow;
				}
				maskIn >>= 1;
				if (maskIn == 0)
				{
					maskIn = 0x80;
					firstRowPtr++;
				}
			}
		}
		if (maskOut != maskOutInit)
		{
			*(dataOutPtr++) = byteOut;
		}
		if (outDataLen != dataOutPtr-rotatedData)
		{
			fprintf(stderr, "Error in CreateRotatedData: outDataLen = %ld, actual = %ld\n", outDataLen, dataOutPtr-rotatedData);
		}
	}
	return(rotatedData);
}

/******************************* CreateFile ***********************************/
int SubsetFontCreator::CreateFile(
	EFormat			inExportFormat,
	const char*		inFontFilePath,
	const char*		inExportPath,
	int32_t			inPointSize,
	int				inOptions,
	const char*		inSubset,
	long			inFontFaceIndex,
	const char*		inSupplementalFontFilePath,
	long			inSupplementalFontFaceIndex,
	std::string*	outErrorStr,
	std::string*	outWarningStr,
	std::string*	outInfoStr)
{
	SubsetCharcodeIterator	charcodeIterator(inSubset, outErrorStr);
	int	createFileError = eSubsetNoErr;
	bool	success = charcodeIterator.IsValid();
	if (success)
	{
		std::string utf8ExportPath(inExportPath);
		if (utf8ExportPath.size())
		{
			FILE* file = inExportFormat == eBinaryXfntFormat ? fopen(utf8ExportPath.c_str(), "w+") : tmpfile();
			if (file)
			{
				bool	glyphDataSeparately = (inOptions & eGlyphDataSeparately) != 0;
				FILE*	glyphDataFile = file;
				if (glyphDataSeparately)
				{
					std::string	glyphDataFilePath(utf8ExportPath);
					const char*	frontPtr = glyphDataFilePath.c_str();
					const char* backPtr = frontPtr + glyphDataFilePath.length() -1;
					for (; *backPtr != '.' && backPtr > frontPtr; --backPtr){}
					glyphDataFilePath.resize(backPtr-frontPtr);
					glyphDataFilePath.append(".gdat");
					glyphDataFile = fopen(glyphDataFilePath.c_str(), "w+");
				}
				if (glyphDataFile)
				{
					createFileError = CreateXfntFile(inFontFilePath, file, glyphDataFile,
										inPointSize, inOptions, charcodeIterator, inFontFaceIndex,
										inSupplementalFontFilePath, inSupplementalFontFaceIndex,
											outErrorStr, outWarningStr, outInfoStr);
					if (inExportFormat != eBinaryXfntFormat &&
						createFileError == eSubsetNoErr)
					{
						FILE*	exportFile = fopen(utf8ExportPath.c_str(), "w+");
						switch (inExportFormat)
						{
							case eC_HeaderFormat:
							{
								std::string	subsetStr;
								charcodeIterator.GetSubset(subsetStr);
								createFileError = XFntToC_Header(file, utf8ExportPath.c_str(), exportFile, subsetStr, !glyphDataSeparately);
								break;
							}
							default:
								break;
						}
						fclose(exportFile);
					}
					if (glyphDataSeparately)
					{
						fclose(glyphDataFile);
					}
				}
				fclose(file);
			}
		}
	} else
	{
		createFileError = charcodeIterator.GetError();
	}
	return(createFileError);
}

/******************************** GetFaceNames ********************************/
int SubsetFontCreator::GetFaceNames(
	const char*					inFontFilePath,
	std::vector<std::string>&	outFaceNames)
{
	int	success = 0;
	std::string utf8FontPath(inFontFilePath);
	FT_Library ftLibrary = NULL;
	if (FT_Init_FreeType(&ftLibrary) == 0)
	{
		FT_Face  face = NULL;
		
		FT_Long  num_faces     = 0;
		FT_Long  num_instances = 0;
		
		FT_Long  face_idx     = 0;
		FT_Long  instance_idx = 0;
		
		
		do
		{
			FT_Long  id = ( instance_idx << 16 ) + face_idx;
			
			success = FT_New_Face(ftLibrary, utf8FontPath.c_str(), id, &face) == 0;
			if (success)
			{
				
				num_faces     = face->num_faces;
				num_instances = face->style_flags >> 16;
				/*
				*	If this isn't a hidden face THEN
				*	add the face name to the array as is
				*/
				if (face->family_name[0] != '.')
				{
					outFaceNames.push_back(face->style_name);
					//fprintf(stderr, "%s - %s\n", face->family_name, face->style_name);
				/*
				*	Else this is a hidden face name  (e.g. ".Lucida Grande UI" is part of Lucida Grande.tcc)
				*/
				} else
				{
					std::string hiddenFaceName(face->family_name);
					hiddenFaceName.append(" - ");
					hiddenFaceName.append(face->style_name);
					outFaceNames.push_back(hiddenFaceName);
				}
				FT_Done_Face( face );
				
				if (instance_idx < num_instances)
				{
					instance_idx++;
				} else
				{
					face_idx++;
					instance_idx = 0;
				}
			} else
			{
				break;
			}
		} while (face_idx < num_faces);
		FT_Done_FreeType(ftLibrary);
	}

	return(success);
}

/******************************* CreateXfntFile *******************************/
int SubsetFontCreator::CreateXfntFile(
	const char*				inFontFilePath,
	FILE*					inExportFile,
	FILE*					inGlyphDataExportFile,	// May be the same as inExportFile
	int32_t					inPointSize,
	int						inOptions,
	SubsetCharcodeIterator&	inCharcodeItr,
	long					inFontFaceIndex,
	const char*				inSupplementalFontFilePath,
	long					inSupplementalFontFaceIndex,
	std::string*			outErrorStr,
	std::string*			outWarningStr,
	std::string*			outInfoStr)
{
	int	createFileError = eSubsetNoErr;
	bool	success = inCharcodeItr.IsValid();
	
	if (success)
	{
		bool	rotated = (inOptions & (e1BitPerPixel+eRotated)) == e1BitPerPixel+eRotated;
		bool	horizontal = (inOptions & eHorizontal) != 0;
		bool	oneBitPerPixel = (inOptions & e1BitPerPixel) != 0;
		bool	wideOffsets = (inOptions & e32BitDataOffsets) != 0;
		bool	minimizeHeight = (inOptions & eMinimizeHeight) != 0;
		GlyphHeader	xGlyphHeader;
		
		if (minimizeHeight)
		{
			FontHeader			xFontHeader;
			int errorCode = PreviewFont(inFontFilePath, inPointSize, inOptions,
								inCharcodeItr, inFontFaceIndex, inSupplementalFontFilePath,
								inSupplementalFontFaceIndex, xFontHeader, xGlyphHeader,
								outErrorStr);
			if (errorCode)
			{
				minimizeHeight = false;
			}/* else
			{
				fprintf(stderr, "ascent = %d\n"
							"descent = %d\n"
							"height = %d\n"
							"width = %d\n"
							"y = %d\n"
							"rows = %d\n",
							(int)xFontHeader.ascent,
							(int)xFontHeader.descent,
							(int)xFontHeader.height,
							(int)xFontHeader.width,
							(int)xGlyphHeader.y,
							(int)xGlyphHeader.rows);
			}*/
		}
		
		std::string utf8FontPath(inFontFilePath);
		std::string utf8SupplementalFontPath(inSupplementalFontFilePath);
		if (inExportFile && inGlyphDataExportFile)
		{
			std::vector<std::string>	faceNameVec;
			GetFaceNames(utf8FontPath.c_str(), faceNameVec);
			
			FT_Library ftLibrary = NULL;
			if (FT_Init_FreeType(&ftLibrary) == 0)
			{
				FT_Face	face = NULL;
				FT_Face	supplementalFace = NULL;
				if (inSupplementalFontFilePath)
				{
					FT_New_Face(ftLibrary, utf8SupplementalFontPath.c_str(), inSupplementalFontFaceIndex, &supplementalFace);
					if (FT_Set_Char_Size(
								supplementalFace,	/* handle to face object           */
								0,		/* char_width in 1/64th of points  */
								inPointSize*64,	/* char_height in 1/64th of points */
								72,		/* horizontal device resolution    */
								72))
					{
						supplementalFace = NULL;
					}
				}
				bool success = FT_New_Face(ftLibrary, utf8FontPath.c_str(), inFontFaceIndex, &face) == 0;
				if (success)
				{
					
					FT_Error error = FT_Set_Char_Size(
								face,	/* handle to face object           */
								0,		/* char_width in 1/64th of points  */
								inPointSize*64,	/* char_height in 1/64th of points */
								72,		/* horizontal device resolution    */
								72);	/* vertical device resolution      */
					
					uint32_t	numCharCodes = inCharcodeItr.GetNumCharCodes();
					FT_Face	charCodeFace = NULL;
					FontHeader fontHeader;
					fontHeader.version = 1;
					//fontHeader.wideOffsets = wideOffsets ? 1:0;
					fontHeader.horizontal = (rotated && horizontal) ? 1:0;
					fontHeader.oneBit = oneBitPerPixel ? 1:0;
					fontHeader.rotated = rotated ? 1:0;
					fontHeader.ascent = face->size->metrics.ascender/64;
					fontHeader.descent = face->size->metrics.descender/64;
					uint32_t	actualFontHeight;
					if (!minimizeHeight)
					{
						actualFontHeight = (uint32_t)(face->size->metrics.height/64);
					} else
					{
						// If there's any kerning the fonts may clip.
						actualFontHeight = xGlyphHeader.rows;
					}
					fontHeader.height = actualFontHeight;
					fontHeader.numCharcodeRuns = inCharcodeItr.GetNumRuns() +1;
					fontHeader.numCharCodes = numCharCodes;
					FT_ULong	maxCharCode = 0;
					uint32_t	maxEntrySize = 0;
					int32_t ascent = (int32_t)(fontHeader.ascent);
					size_t glyphDataOffsetsSize = (numCharCodes + 1) * (wideOffsets ? sizeof(uint32_t) : sizeof(uint16_t));
					uint32_t*	glyphDataOffsets = new uint32_t[numCharCodes+1];
					uint32_t*	glyphDataOffsets32Ptr = glyphDataOffsets;
					uint16_t*	glyphDataOffsets16Ptr = (uint16_t*)glyphDataOffsets;
					uint8_t		widestGlyph = 0;
					uint8_t		tallestGlyph = 0;
					uint8_t		minGlyphY = 255;
					uint8_t		isMonospaced = 1;
					bool		containsKerning = false;
					// Reserve space for the header
					fseek(inExportFile, sizeof(FontHeader), SEEK_CUR);
					size_t charcodeRunsSize;
					/*
					*	Create and write the CharcodeRuns array.
					*/
					{
						const IndexVec&	indexVec = inCharcodeItr.GetIndexVec();
						const Runs&	charCodeRuns = indexVec.GetRuns();
						// The size of the charCodeRuns array is either odd or even.
						// Either way dividing by 2 gives you the number of runs.
						// (see IndexVec.h for more info)
						size_t	numRuns = (charCodeRuns.size()/2) + 1;  // +1 accounts for the null last run used for sanity checking (see header).
						CharcodeRun*	charcodeRuns = new CharcodeRun[numRuns];
						CharcodeRun*	charcodeRunsPtr = charcodeRuns;
					
						{
							uint16_t	entryIndex = 0;
							Runs::const_iterator	itr = charCodeRuns.begin();
							Runs::const_iterator	itrEnd = charCodeRuns.end();

							if (indexVec.GetFirstRunValue() == 0)
							{
								++itr;
							}
							
							for (; itr != itrEnd; ++itr)
							{
								charcodeRunsPtr->start = *itr;
								++itr;
								charcodeRunsPtr->entryIndex = entryIndex;
								entryIndex += (*itr - charcodeRunsPtr->start);
								charcodeRunsPtr++;
							}
							// Add the null last run used for sanity checking (see header).
							charcodeRunsPtr->start = 0xFFFF;
							charcodeRunsPtr->entryIndex = entryIndex;
						}
						charcodeRunsSize = sizeof(CharcodeRun) * numRuns;
						fwrite(charcodeRuns, charcodeRunsSize, 1, inExportFile);
						delete [] charcodeRuns;
					}
					// Reserve space for the glyph data offsets
					fseek(inExportFile, glyphDataOffsetsSize, SEEK_CUR);
					uint32_t	glyphDataOffset = 0;

					if (error == 0)
					{
						GlyphHeader	glyphHdr;
						FT_ULong	charcode;
						FT_Int32	loadFlags = FT_LOAD_RENDER;
						if (oneBitPerPixel)
						{
							loadFlags |= FT_LOAD_TARGET_MONO;
						}	// else grayscale
						charcode = inCharcodeItr.Current();
						while (inCharcodeItr.IsValid() &&
							createFileError == eSubsetNoErr)
						{
							if (charcode > 0 && charcode < 0xFFFF)
							{
								FT_UInt	glyphIndex = FT_Get_Char_Index(face, charcode);
								if (glyphIndex ||
									supplementalFace == NULL)
								{
									error = FT_Load_Glyph(face, glyphIndex, loadFlags);
									charCodeFace = face;
								} else
								{
									error = FT_Load_Char(supplementalFace, charcode, loadFlags);
									charCodeFace = supplementalFace;
								}
								if (error == 0)
								{
									if (maxCharCode < charcode)
									{
										maxCharCode = charcode;
									}
									FT_GlyphSlot	slot = charCodeFace->glyph;
									FT_Bitmap&		bitmap = slot->bitmap;
									int32_t			rows = bitmap.rows;
									int32_t			width = 0;
									int32_t			linePadding = 0;
									const uint8_t*	bufferPtr = bitmap.buffer;
									uint8_t*		glyphData = NULL;
									uint8_t*		glyphDataPtr = NULL;
									if (oneBitPerPixel)
									{
										if (rotated)
										{
											size_t	rotatedDataLen;
											// Rotated data is created with MSB bottom for use by SSD1306 and PCD8544 controllers.
											glyphData = CreateRotatedData(bufferPtr, rows, bitmap.width, ascent - slot->bitmap_top, false, horizontal, rotatedDataLen);
											if (glyphData)
											{
												glyphDataPtr = &glyphData[rotatedDataLen];
											}
											glyphHdr.rows = rows; // Write the actual number of rows rather than the number of bytes per row
											glyphHdr.columns = bitmap.width;
										} else
										{
											width = (bitmap.width + 7)/8;
											linePadding = width & 1;
											glyphHdr.rows = rows;
											glyphHdr.columns = bitmap.width; // Write the actual number of columns rather than the number of bytes per column
										}
									} else
									{
										width = bitmap.width;
										linePadding = bitmap.pitch - bitmap.width;
										glyphHdr.rows = rows;
										glyphHdr.columns = width;
									}
									if (!rotated)
									{
										int32_t		glyphDataMaxSize = ((width<<1)*rows) + 2;  // +2 allows for 2 bytes at the end (needed for grayscale only)
										glyphData = new uint8_t[glyphDataMaxSize];	// worst case size.
										glyphDataPtr = glyphData;
									}
									uint8_t		advanceX = slot->advance.x/64;
									glyphHdr.advanceX = advanceX;
									glyphHdr.x = slot->bitmap_left;
									glyphHdr.y = ascent - slot->bitmap_top;
									if (minimizeHeight)
									{
										glyphHdr.y -= xGlyphHeader.y;
									}
									if (glyphHdr.x < 0 ||
										glyphHdr.y < 0)
									{
										containsKerning = true;
									}
									if (advanceX < abs(glyphHdr.x) + glyphHdr.columns)
									{
										containsKerning = true;
										advanceX = abs(glyphHdr.x) + glyphHdr.columns;
									}
									if (tallestGlyph < glyphHdr.rows)
									{
										tallestGlyph = glyphHdr.rows;
									}
									
									if (minGlyphY > glyphHdr.y)
									{
										minGlyphY = glyphHdr.y;
									}
									
									if (advanceX != widestGlyph)
									{
										if (widestGlyph)
										{
											isMonospaced = 0;
										}
										
										if (advanceX > widestGlyph)
										{
											widestGlyph = advanceX;
										}
									}
									// Write the glyph header (but don't increment glyphDataOffset till after the runs are written)
									fwrite(&glyphHdr, sizeof(GlyphHeader), 1, inGlyphDataExportFile);
									
									if (rows)
									{
										/*
										*	If this is 1 bit data THEN
										*	copy and/or pack the glyph data.
										*/
										if (oneBitPerPixel)
										{
											// If not rotated (rotated data is already in the glyphData mem) AND
											if (!rotated)
											{
												/*
												*	1 bit unrotated is stored as rows of serial horizontal bits.  The data for one
												*	1 bit high row followed by the next row...
												*	Rotated is stored as vertical 8 bit high columns.  The data for one
												*	complete column, then followed by the next column...
												*
												*	Storage example for the letter 'L':
												*	For unrotated it's stored as:	xx...
												*									xx...
												*									xx...
												*									xx...
												*									xxxxx
												*	For rotated it's stored as:
												*									xxxxx
												*									xxxxx
												*									....x
												*									....x
												*									....x
												*
												*	To compress unrotated, the padding bits at the end of each
												*	horizontal strip is remove and the data that follows is
												*	shifted left.
												*
												*	In the example below, the pixel width for each row is 5.
												*	For 3 bytes aaaaa... bbbbb... ccccc... becomes 2 bytes aaaaabbb bbccccc after packing.
												*
												*	Rotated data is compressed in CreateRotatedData.  See CreateRotatedData for details.
												*/
												uint32_t	bitsPerRow = glyphHdr.columns;
												/*
												*	packing is needed...
												*/
												if (bitsPerRow & 7)
												{
													uint32_t	bitsInByteIn = 0;
													uint32_t	bitsInByteOut = 0;
													uint8_t		byteIn = 0;
													uint8_t		byteOut = 0;
													for (int row = 0; row < rows; row++)
													{
														uint32_t	bitsInRow = bitsPerRow;
														while (bitsInRow)
														{
															if (bitsInByteIn == 0)
															{
																if (bitsInRow >= 8)
																{
																	bitsInByteIn = 8;
																	bitsInRow -= 8;
																} else
																{
																	bitsInByteIn = bitsInRow;
																	bitsInRow = 0;
																}
																byteIn = *(bufferPtr++);
															}
															if (bitsInByteOut)
															{
																byteOut |= (byteIn >> bitsInByteOut);
																uint32_t	bitsNeededToFillOut = 8-bitsInByteOut;
																if (bitsNeededToFillOut < bitsInByteIn)
																{
																	byteIn <<= bitsNeededToFillOut;
																	bitsInByteIn -= bitsNeededToFillOut;
																	*(glyphDataPtr++) = byteOut;
																	bitsInByteOut = 0;
																} else
																{
																	if (bitsNeededToFillOut > bitsInByteIn)
																	{
																		bitsInByteOut += bitsInByteIn;
																	} else
																	{
																		*(glyphDataPtr++) = byteOut;
																		bitsInByteOut = 0;
																	}
																	bitsInByteIn = 0;
																}
															} else
															{
																byteOut = byteIn;
																if (bitsInByteIn < 8)
																{
																	bitsInByteOut = bitsInByteIn;
																} else
																{
																	*(glyphDataPtr++) = byteOut;
																}
																bitsInByteIn = 0;
															}
														}
														if (linePadding)
														{
															bufferPtr += linePadding;
														}
													}
													if (bitsInByteOut)
													{
														*(glyphDataPtr++) = byteOut;
													} else if (bitsInByteIn)
													{
														*(glyphDataPtr++) = byteIn;
													}
													
												// Else, packing is not needed (all bits are being used)
												} else
												{
													for (int row = 0; row < rows; row++)
													{
														memcpy(glyphDataPtr, bufferPtr, width);
														bufferPtr += width;
														glyphDataPtr += width;
														if (linePadding)
														{
															bufferPtr += linePadding;
														}
													}
												}
											}
										/*
										*	Else run length encode.  Runs of the same value have a positve run length
										*	between 3 and 127.  Runs of unique values have a negative run length between
										*	-1 and -128.
										*/
										} else
										{
											uint8_t		pixel;
											uint8_t		runPixel;
											int32_t		runLen;
											int32_t		uniqueRunLen;
											uint8_t*	runLenPtr;
											//for (int row = 0; row < rows; row++)	<< uncomment if the runs need to break at the end of each row
											{
												runPixel = *(bufferPtr++);
												runLen = 1;
												uniqueRunLen = 1;
												runLenPtr = glyphDataPtr;
												glyphDataPtr++;
												*(glyphDataPtr++) = runPixel;
												int		dataLen = rows * width;
												for (int dataIndex = 1; dataIndex < dataLen; dataIndex++)
												//for (int col = 1; col < width; col++)	<< uncomment if the runs need to break at the end of each row
												{
													pixel = *(bufferPtr++);
													if (runPixel == pixel)
													{
														runLen++;
														switch(runLen)
														{
															/*
															*	Until the runLen is 3 it can still be treated as
															*	a run of unique pixels, so keep appending and counting pixels.
															*/
															case 2:
																*(glyphDataPtr++) = runPixel;
																uniqueRunLen++;
																break;
															/*
															*	Once the runLen is 3 the unique run is terminated, if any.
															*/
															case 3:
																uniqueRunLen++;
																if (uniqueRunLen > runLen)
																{
																	uniqueRunLen -= runLen;
																	*runLenPtr = -uniqueRunLen; // write a negative value
																	runLenPtr += (uniqueRunLen +1);
																	uniqueRunLen = 0;	// not needed, for debugging.
																}
																break;
															/*
															*	The maximum length of a run is 127.  Start a new run at 128
															*/
															case 128:
																*runLenPtr = 127;
																runLenPtr += 2;
																glyphDataPtr = runLenPtr + 1;
																*(glyphDataPtr++) = runPixel;
																runLen = 1;
																uniqueRunLen = 1;
																break;
															default:
																break;
														}
													/*
													*	Else the run just ended.
													*	If the runLen is more than 2 THEN
													*	record the run length and start a new run.
													*/
													} else
													{
														if (runLen > 2)
														{
															*runLenPtr = runLen;
															runLenPtr += 2;
															glyphDataPtr = runLenPtr + 1;
															runLen = 1;
															uniqueRunLen = 1;
														} else
														{
															runLen = 1;
															if (uniqueRunLen != 128)
															{
																uniqueRunLen++;
															} else
															{
																*runLenPtr = -128;
																runLenPtr = glyphDataPtr;
																glyphDataPtr++;
																uniqueRunLen = 1;
															}
														}
														*(glyphDataPtr++) = pixel;
														runPixel = pixel;
													}
												}
												if (runLen > 2)
												{
													*runLenPtr = runLen;
													glyphDataPtr = runLenPtr + 2;
													*glyphDataPtr = 0xFF;
												} else if (uniqueRunLen)
												{
													*runLenPtr = -uniqueRunLen;
												} else
												{
													createFileError = eUnexpectedEndOfRunErr;
													if (outErrorStr)
													{
														outErrorStr->assign("Unexpected end of run");
													}
												}
												
												if (linePadding)
												{
													bufferPtr += linePadding;
												}
											}
										}
									}
									uint32_t bytesWritten = (uint32_t)(glyphDataPtr - glyphData);
									fwrite(glyphData, bytesWritten, 1, inGlyphDataExportFile);
									if (wideOffsets)
									{
										*(glyphDataOffsets32Ptr++) = glyphDataOffset;
									} else if (glyphDataOffset < 0x10000)
									{
										*(glyphDataOffsets16Ptr++) = (uint16_t)glyphDataOffset;
									} else
									{
										createFileError = eDataOffsetTooLargeErr;
										if (outErrorStr)
										{
											outErrorStr->assign("Data offset too wide - needs 32 bit.");
										}
									}
									{
										uint32_t thisEntrySize = bytesWritten + sizeof(GlyphHeader);
										glyphDataOffset += thisEntrySize;
										if (thisEntrySize > maxEntrySize)
										{
											maxEntrySize = thisEntrySize;
										}
									}
									if (glyphData)
									{
										delete [] glyphData;
										glyphData = NULL;
									}
								} else
								{
									createFileError = eFTLoadCharFailedErr;
									if (outErrorStr)
									{
										outErrorStr->assign("FT_Load_Char failed.");
									}
								}
							}
							charcode = inCharcodeItr.Next();
						}
						if (wideOffsets)
						{
							*glyphDataOffsets32Ptr = glyphDataOffset;	// offset of the end of all glyph data
						} else if (glyphDataOffset < 0x10000)
						{
							*glyphDataOffsets16Ptr = (uint16_t)glyphDataOffset;	// offset of the end of all glyph data
						} else
						{
							createFileError = eDataOffsetTooLargeErr;
							if (outErrorStr)
							{
								outErrorStr->assign("Data offset too wide - needs 32 bit.");
							}
						}
					} else
					{
						createFileError = eFTSetCharSizeFailedErr;
						if (outErrorStr)
						{
							outErrorStr->assign("FT_Set_Char_Size() failed.");
						}
					}
					
					// Write the header
					fseek(inExportFile, 0, SEEK_SET);
					fontHeader.width = widestGlyph;	// Used to simulate monospace on non-monospace fonts
					fontHeader.monospaced = isMonospaced;	// Entire subset has same width+advanceX
					fwrite(&fontHeader, sizeof(FontHeader), 1, inExportFile);
					// Write the glyph data offsets
					fseek(inExportFile, sizeof(FontHeader)+charcodeRunsSize, SEEK_SET);
					fwrite(glyphDataOffsets, glyphDataOffsetsSize, 1, inExportFile);
					fseek(inExportFile, 0, SEEK_END);
					if (outInfoStr)
					{
						char infoBuff[1024];
						std::string	subsetStr;
						inCharcodeItr.GetSubset(subsetStr);
						
						outInfoStr->append(infoBuff, snprintf(infoBuff, 1024,
							"\nSubset string = \"%s\"\n"
							"Charcode count = %d\n"
							"Run count = %d\n"
							"Max charcode = %04XU\n"
							"Monospace = %s\n"
							"Widest glyph = %d\n"
							"Tallest glyph = %d\n"
							"Min glyph.y = %d\n"
							"Height = %dpx, or %d 1-bit rows\n"
							"Glyph data length = %d\n"
							"Largest glyph length = %d\n",
							subsetStr.c_str(),
							numCharCodes,
							inCharcodeItr.GetNumRuns(),
							(uint32_t)maxCharCode,
							isMonospaced ? "true" : "false",
							(int)widestGlyph,
							(int)tallestGlyph,
							(int)minGlyphY,
							(int)fontHeader.height,
							(int)(fontHeader.height + 7)/8,
							(uint32_t)glyphDataOffset,
							maxEntrySize));
					}
					if (outWarningStr)
					{
						if (containsKerning)
						{
							outWarningStr->append(
								"Some glyphs in this font apply kerning (when a glyph's x or y is negative, or the "
								"abs(x) + columns is greater than the advance X.)  "
								"XFont handles kerning by zeroing the x and/or increasing the advance X.");
						}
					}
					if (outErrorStr)
					{
						if (fontHeader.height != actualFontHeight)
						{
							outErrorStr->assign("Font size too large.  "
								"Resulting font height is greater than 255.  "
								"Choose a smaller font size.");
						}
					}
					delete [] glyphDataOffsets;
					FT_Done_Face(face);
				} else
				{
					createFileError = eFTNewFaceFailedErr;
					if (outErrorStr)
					{
						outErrorStr->assign("FT_New_Face() failed.");
					}
				}
				if (supplementalFace)
				{
					FT_Done_Face(supplementalFace);
				}
				FT_Done_FreeType(ftLibrary);
			} else
			{
				createFileError = eFTInitFreeTypeFailedErr;
				if (outErrorStr)
				{
					outErrorStr->assign("FT_Init_FreeType() failed.");
				}
			}
		}
	} else
	{
		createFileError = inCharcodeItr.GetError();
	}
	return(createFileError);
}

/**************************** CleanStrForMacroName ****************************/
/*
*	This routine assumes a valid UTF-8 string is passed.
*/
void CleanStrForMacroName(
	const char*		inStr,
	std::string&	outStr)
{
	const char*	thisCharPtr = inStr;
	const char*	substringStart = inStr;
	char	thisChar = *thisCharPtr;
	
	/*
	*	If the first character isn't an alpha character (US locale) THEN
	*	Insert an 'M' as the first character
	*/
	if (thisChar < 'A' || thisChar > 'z' ||
		(thisChar > 'Z' && thisChar < 'a'))
	{
		outStr += 'M';
	}
	
	/*
	*	Go through all of the characters and replace any illegal characters
	*	with an underscore.
	*/
	for (; thisChar != 0; thisChar = *(++thisCharPtr))
	{
		/*
		*	If the character is valid THEN
		*	continue.
		*/
		if ((thisChar >= 'a' && thisChar <= 'z') ||
			(thisChar >= 'A' && thisChar <= 'Z') ||
			(thisChar >= '0' && thisChar <= '9') ||
			thisChar == '_')
		{
			continue;
		}
		size_t	substrLen = thisCharPtr-substringStart;
		if (substrLen)
		{
			outStr.append(substringStart, substrLen);
		}
		/*
		*	If this is a multibyte UTF-8 character THEN
		*	skip the entire character.
		*/
		if ((thisChar & 0xC0) == 0xC0)
		{
			thisCharPtr++;
			if (thisChar & 0x20)
			{
				thisCharPtr++;
				if (thisChar & 0x10)
				{
					thisCharPtr++;
				}
			}
		}
		substringStart = thisCharPtr + 1;
		outStr += '_';
	}
	if (thisCharPtr > substringStart)
	{
		outStr.append(substringStart, thisCharPtr-substringStart);
	}
}

/**************************** GetLastPathComponent ****************************/
/*
*	Returns a pointer to the last path component within inPath.
*/
const char* GetLastPathComponent(
	const char*	inPath)
{
	const char*	lastPathComponent = inPath;
	char		thisChar = *(inPath++);
	for (; thisChar != 0; thisChar = *(inPath++))
	{
		if (thisChar != '/')
		{
			continue;
		}
		lastPathComponent = inPath;
	}
	return(lastPathComponent);
}

/***************************** XFntToC_Header *********************************/
int SubsetFontCreator::XFntToC_Header(
	FILE*			inXFntFile,
	const char*		inExportPath,
	FILE*			inOutputFile,
	std::string&	inSubsetStr,
	bool			inIncludeGlyphData)
{
	TabbedFileStream	tabbedStream(25, inOutputFile);
	fseek(inXFntFile, 0, SEEK_END);
	long fileSize = ftell(inXFntFile);
	uint8_t*	xfntBuf = new uint8_t[fileSize];
	fseek(inXFntFile, 0, SEEK_SET);
	fread(xfntBuf, 1, fileSize, inXFntFile);
	FontHeader* 	fontHeader;
	fontHeader = (FontHeader*)xfntBuf;
	std::string	headerMacro;
	const char*	exportFilename = GetLastPathComponent(inExportPath);
	uint32_t	glyphDataLen = 0;
	CleanStrForMacroName(exportFilename, headerMacro);
	// For the namespace name, strip off _h if it exists (it should always exist)
	std::string	namespaceName(headerMacro, 0, headerMacro.length() - 2);
	std::string	xFontStreamClassName(fontHeader->rotated ?
		(fontHeader->horizontal ? "XFontRH1BitDataStream" : "XFontR1BitDataStream") :
			"XFont16BitDataStream");
	tabbedStream.Write(
		"// Subset font created by SubsetFontCreator"
		"\n// For subset: \"%s\""
		"\n\n#ifndef %s"
		"\n#define %s"
		"\n\n#include \"XFontGlyph.h\""
		"\n#include \"%s.h\""
		"\n\nnamespace %s"
		"\n{"
		"\n\tconst FontHeader\tfontHeader PROGMEM ="
		"\n\t{", inSubsetStr.c_str(),
				 headerMacro.c_str(),
				 headerMacro.c_str(),
				 xFontStreamClassName.c_str(),
				 namespaceName.c_str());
	tabbedStream+=2;
		//	32 bit offsets are not supported in C headers.
		//"\n%d,%2t// wideOffsets, 1 = 32 bit, 0 = 16 bit glyph data offsets"
		//(int)fontHeader->wideOffsets,
	tabbedStream.Write(
		"\n%d,%2t// version, currently version = 1"
		"\n%d,%2t// oneBit, 1 = 1 bit per pixel, 0 = 8 bit (antialiased)"
		"\n%d,%2t// rotated, glyph data is rotated (applies to 1 bit only)"
		"\n%d,%2t// horizontal, addressing for rotated data, else vertical"
		"\n%d,%2t// monospaced, fixed width font (for this subset)"
		"\n%d,%2t// ascent, font in pixels"
		"\n%d,%2t// descent, font in pixels"
		"\n%d,%2t// height, font height (ascent+descent+leading) in pixels"
		"\n%d,%2t// width, widest glyph advanceX within subset in pixels"
		"\n%d,%2t// numCharcodeRuns"
		"\n%d%2t// numCharCodes",
		(int)fontHeader->version,
		(int)fontHeader->oneBit,
		(int)fontHeader->rotated,
		(int)fontHeader->horizontal,
		(int)fontHeader->monospaced,
		(int)fontHeader->ascent,
		(int)fontHeader->descent,
		(int)fontHeader->height,
		(int)fontHeader->width,
		(int)fontHeader->numCharcodeRuns,
		(int)fontHeader->numCharCodes);
	tabbedStream--;
	tabbedStream.Write(
		"\n};"
		"\n\nconst CharcodeRun\tcharcodeRun[] PROGMEM = // {start, entryIndex}, ..."
		"\n{");
	tabbedStream++;
	uint8_t*	currOffset = &xfntBuf[sizeof(FontHeader)];
	{
		CharcodeRun*	runPtr = (CharcodeRun*)currOffset;
		CharcodeRun*	runEnd = &runPtr[fontHeader->numCharcodeRuns];
		currOffset = (uint8_t*)runEnd;
		
		int	runsOnRow = 0;
		while (runPtr != runEnd)
		{
			if (runsOnRow == 0)
			{
				tabbedStream.Write("\n");
			}
			
			fprintf(inOutputFile, "{0x%04hX, %hd}", runPtr->start, runPtr->entryIndex);
			runPtr++;
			runsOnRow++;
			if (runsOnRow == 5)	// runs per row
			{
				runsOnRow = 0;
			}
			if (runPtr != runEnd)
			{
				fprintf(inOutputFile, runsOnRow != 0 ? ", " : ",");
			}
		}
	}
	tabbedStream--;
	tabbedStream.Write(
		"\n};"
		"\n\nconst uint16_t\tglyphDataOffset[] PROGMEM ="
		"\n{");
	tabbedStream++;
	{
		uint16_t*	dataOffsetPtr = (uint16_t*)currOffset;
		uint16_t*	dataOffsetEnd = &dataOffsetPtr[fontHeader->numCharCodes+1];
		currOffset = (uint8_t*)dataOffsetEnd;
		glyphDataLen = dataOffsetEnd[-1];
		
		int	offsetsOnRow = 0;
		while (dataOffsetPtr != dataOffsetEnd)
		{
			if (offsetsOnRow == 0)
			{
				tabbedStream.Write("\n");
			}
			
			fprintf(inOutputFile, "0x%04hX", *dataOffsetPtr);
			dataOffsetPtr++;
			offsetsOnRow++;
			if (offsetsOnRow == 8)	// offsets per row
			{
				offsetsOnRow = 0;
			}
			if (dataOffsetPtr != dataOffsetEnd)
			{
				fprintf(inOutputFile, offsetsOnRow != 0 ? ", " : ",");
			}
		}
	}
	tabbedStream--;
	tabbedStream.Write(
		"\n};");
	if (inIncludeGlyphData)
	{
		tabbedStream.Write(
			"\n\nconst uint8_t\tglyphData[] PROGMEM ="
			"\n{");
		tabbedStream++;
		{
			const uint8_t*	dataPtr = currOffset;
			const uint8_t*	endData = (uint8_t*)&xfntBuf[fileSize];
			int	valuesOnRow = 0;
			while (dataPtr != endData)
			{
				if (valuesOnRow == 0)
				{
					tabbedStream.Write("\n");
				}
				
				fprintf(inOutputFile, "0x%02hhX", *dataPtr);
				dataPtr++;
				valuesOnRow++;
				if (valuesOnRow == 12)	// bytes per row
				{
					valuesOnRow = 0;
				}
				if (dataPtr != endData)
				{
					fprintf(inOutputFile, valuesOnRow != 0 ? ", " : ",");
				}
			}
#if 0
// For debugging a single character of a 1 bit glyph
		dataPtr = currOffset;
		Glyph*	glyph = (Glyph*)dataPtr;
		tabbedStream.Write(
			"\n"
			"\n%d,%2t// advanceX"
			"\n%d,%2t// x"
			"\n%d,%2t// y"
			"\n%d,%2t// rows"
			"\n%d,%2t// columns\n",
			(int)glyph->advanceX,
			(int)glyph->x,
			(int)glyph->y,
			(int)glyph->rows,
			(int)glyph->columns);
		dataPtr += sizeof(Glyph);
		uint8_t	binaryStr[32];
		uint32_t	valuesPerRow = fontHeader->rotated ? (glyph->rows+7)/8 : (glyph->columns+7)/8;
		valuesOnRow = 0;
		while (dataPtr != endData)
		{
			if (valuesOnRow == 0)
			{
				tabbedStream.Write("\n");
			}
		
			uint8_t	valueIn = *(dataPtr++);
			uint8_t	nMask = 0x80;
			uint8_t*	buffPtr = binaryStr;
			for (; nMask != 0; nMask >>= 1)
			{
				*(buffPtr++) = (valueIn & nMask) ? '1':'0';
				if (nMask & 0xEF)
				{
					continue;
				}
				*(buffPtr++) = ' ';
			}
			*buffPtr = 0;
			fprintf(inOutputFile, "0x%02hhX %s", valueIn, binaryStr);

			valuesOnRow++;
			if (valuesOnRow == valuesPerRow)
			{
				valuesOnRow = 0;
			}
			if (dataPtr != endData)
			{
				fprintf(inOutputFile, valuesOnRow != 0 ? ", " : ",");
			}
		}
#endif
		}
		tabbedStream--;
		tabbedStream.Write(
			"\n};");
	tabbedStream.Write(
		"\n\n// Usage: Copy the 3 commented lines to your code, removing the comments."
		"\n//#include \"XFont.h\""
		"\n//XFont xFont;"
		"\n//#include \"%s\""
		"\n\n// Leave the next 3 lines here, as is."
		"\nDataStream_P\tdataStream(glyphData, sizeof(glyphData));"
		"\n%s xFontDataStream(&xFont, &dataStream);"
		"\nXFont::Font font(&fontHeader, charcodeRun, glyphDataOffset, &xFontDataStream);"
		"\n\n// The display needs to be set before using xFont.  This only needs"
		"\n// to be done once at the beginning of the program."
		"\n// Use xFont.SetDisplay(&display, &%s::font); to do this."
		"\n// To change to this font anywhere after setting the display,"
		"\n// use: xFont.SetFont(&%s::font);",
		exportFilename,
		xFontStreamClassName.c_str(),
		namespaceName.c_str(),
		namespaceName.c_str());
	} else
	{
		tabbedStream.Write(
			"\n\nconst uint32_t\tkGlyphDataLength = %d;", glyphDataLen);
	}
	tabbedStream--;
	tabbedStream.Write(
		"\n}"
		"\n\n#endif // %s\n\n",
		headerMacro.c_str());
	delete [] xfntBuf;
	return(0);
}

/******************************** PreviewFont *********************************/
/*
*	This routine was implemented to support the Minimize Height feature. When
*	enabled, Minimize Height trims the font ascent descent and overall font
*	height.  It also adjusts each GlyphHeader.y by the amount removed from the
*	ascent.
*	This only is useful on horizontal fonts.
*	outFontHeader is filled in as if the header were to be written to a file,
*	except for monospaced and width which are set to 0.
*	outGlyphHeader contains the maximum values except for GlyphHeader.y.
*	GlyphHeader.y contains the minimum value.  GlyphHeader.y is used to
*	determine how much the GlyphHeader.y and ascent can be trimmed.
*/
int SubsetFontCreator::PreviewFont(
	const char*				inFontFilePath,
	int32_t					inPointSize,
	int						inOptions,
	SubsetCharcodeIterator&	inCharcodeItr,
	long					inFontFaceIndex,
	const char*				inSupplementalFontFilePath,
	long					inSupplementalFontFaceIndex,
	FontHeader&				outFontHeader,
	GlyphHeader&			outGlyphHeader,
	std::string*			outErrorStr)
{
	int	createFileError = eSubsetNoErr;
	bool	success = inCharcodeItr.IsValid();
	
	outGlyphHeader.advanceX = 0;
	outGlyphHeader.x = 0;
	outGlyphHeader.y = 127;
	outGlyphHeader.rows = 0;
	outGlyphHeader.columns = 0;
	
	if (success)
	{
		bool	rotated = (inOptions & (e1BitPerPixel+eRotated)) == e1BitPerPixel+eRotated;
		bool	horizontal = (inOptions & eHorizontal) != 0;
		bool	oneBitPerPixel = (inOptions & e1BitPerPixel) != 0;
		std::string utf8FontPath(inFontFilePath);
		std::string utf8SupplementalFontPath(inSupplementalFontFilePath);
		{
			std::vector<std::string>	faceNameVec;
			GetFaceNames(utf8FontPath.c_str(), faceNameVec);
			
			FT_Library ftLibrary = NULL;
			if (FT_Init_FreeType(&ftLibrary) == 0)
			{
				FT_Face	face = NULL;
				FT_Face	supplementalFace = NULL;
				if (inSupplementalFontFilePath)
				{
					FT_New_Face(ftLibrary, utf8SupplementalFontPath.c_str(), inSupplementalFontFaceIndex, &supplementalFace);
					if (FT_Set_Char_Size(
								supplementalFace,	/* handle to face object           */
								0,		/* char_width in 1/64th of points  */
								inPointSize*64,	/* char_height in 1/64th of points */
								72,		/* horizontal device resolution    */
								72))
					{
						supplementalFace = NULL;
					}
				}
				bool success = FT_New_Face(ftLibrary, utf8FontPath.c_str(), inFontFaceIndex, &face) == 0;
				if (success)
				{
					
					FT_Error error = FT_Set_Char_Size(
								face,	/* handle to face object           */
								0,		/* char_width in 1/64th of points  */
								inPointSize*64,	/* char_height in 1/64th of points */
								72,		/* horizontal device resolution    */
								72);	/* vertical device resolution      */
					
					uint32_t	numCharCodes = inCharcodeItr.GetNumCharCodes();
					FT_Face	charCodeFace = NULL;
					outFontHeader.version = 1;
					outFontHeader.horizontal = (rotated && horizontal) ? 1:0;
					outFontHeader.oneBit = oneBitPerPixel ? 1:0;
					outFontHeader.rotated = rotated ? 1:0;
					outFontHeader.ascent = face->size->metrics.ascender/64;
					outFontHeader.descent = face->size->metrics.descender/64;
					outFontHeader.height = (uint32_t)(face->size->metrics.height/64);;
					outFontHeader.numCharcodeRuns = inCharcodeItr.GetNumRuns() +1;
					outFontHeader.numCharCodes = numCharCodes;
					outFontHeader.monospaced = 0;
					outFontHeader.width = 0;
					uint32_t	maxHeight = 0;
					int32_t ascent = (int32_t)(outFontHeader.ascent);

					if (error == 0)
					{
						FT_ULong	charcode;
						FT_Int32	loadFlags = FT_LOAD_RENDER;
						if (oneBitPerPixel)
						{
							loadFlags |= FT_LOAD_TARGET_MONO;
						}	// else grayscale
						charcode = inCharcodeItr.Current();
						while (inCharcodeItr.IsValid() &&
							createFileError == eSubsetNoErr)
						{
							if (charcode > 0 && charcode < 0xFFFF)
							{
								FT_UInt	glyphIndex = FT_Get_Char_Index(face, charcode);
								if (glyphIndex ||
									supplementalFace == NULL)
								{
									error = FT_Load_Glyph(face, glyphIndex, loadFlags);
									charCodeFace = face;
								} else
								{
									error = FT_Load_Char(supplementalFace, charcode, loadFlags);
									charCodeFace = supplementalFace;
								}
								if (error == 0)
								{
									FT_GlyphSlot	slot = charCodeFace->glyph;
									FT_Bitmap&		bitmap = slot->bitmap;
									int32_t			rows = bitmap.rows;


									if (outGlyphHeader.x < slot->bitmap_left)
									{
										outGlyphHeader.x = slot->bitmap_left;
									}
									int8_t	thisGlyphY = ascent - slot->bitmap_top;
									if (outGlyphHeader.y > thisGlyphY)
									{
										outGlyphHeader.y = thisGlyphY;
									}
									if (outGlyphHeader.rows < rows)
									{
										outGlyphHeader.rows = rows;
									}
									if (maxHeight < (rows + thisGlyphY))
									{
										maxHeight = rows + thisGlyphY;
									}
									if (outGlyphHeader.columns < bitmap.width)
									{
										outGlyphHeader.columns = bitmap.width;
									}

									uint8_t		advanceX = slot->advance.x/64;
									if (outGlyphHeader.advanceX < advanceX)
									{
										outGlyphHeader.advanceX = advanceX;
									}
								} else
								{
									createFileError = eFTLoadCharFailedErr;
									if (outErrorStr)
									{
										outErrorStr->assign("FT_Load_Char failed.");
									}
								}
							}
							charcode = inCharcodeItr.Next();
						}
						maxHeight -= outGlyphHeader.y;
						if (maxHeight > outGlyphHeader.rows)
						{
							outGlyphHeader.rows = maxHeight;
						}
					} else
					{
						createFileError = eFTSetCharSizeFailedErr;
						if (outErrorStr)
						{
							outErrorStr->assign("FT_Set_Char_Size() failed.");
						}
					}
					FT_Done_Face(face);
				} else
				{
					createFileError = eFTNewFaceFailedErr;
					if (outErrorStr)
					{
						outErrorStr->assign("FT_New_Face() failed.");
					}
				}
				if (supplementalFace)
				{
					FT_Done_Face(supplementalFace);
				}
				FT_Done_FreeType(ftLibrary);
			} else
			{
				createFileError = eFTInitFreeTypeFailedErr;
				if (outErrorStr)
				{
					outErrorStr->assign("FT_Init_FreeType() failed.");
				}
			}
		}
		inCharcodeItr.MoveToStart();
	} else
	{
		createFileError = inCharcodeItr.GetError();
	}
	return(createFileError);
}

/************************* SubsetCharcodeIterator *****************************/
SubsetCharcodeIterator::SubsetCharcodeIterator(
	const char*		inSubset,
	std::string*	outErrorStr)
	: mIsValid(false), mErrorStr(outErrorStr)
{
	mError = eEmptySubsetErr;
	if (inSubset)
	{
		Initialize(inSubset);
	}
}

/******************************* Initialize ***********************************/
/*
*	The subset ranges are sorted lowest to highest so that the charcodes can be
*	searched non linearly.  Ranges that overlap are combined.
*/
void SubsetCharcodeIterator::Initialize(
	const char*	inSubset)
{
	std::string	subset(inSubset);
	if (subset.length())
	{
		mError = eSubsetNoErr;
		mSubsetVec.Clear();
		std::string::const_iterator	strItr = subset.begin();
		std::string::const_iterator	strItrEnd = subset.end();
		/*
		*	Validate the ranges
		*	- there must be an end range
		*	- the start of each range must be <= the end of the range
		*/
		uint32_t	startChar = 0;
		int			pairIndex = 0;	// Used for error reporting
		for (startChar = NextChar(strItr, strItrEnd); startChar; startChar = NextChar(strItr, strItrEnd))
		{
			uint32_t	endChar = NextChar(strItr, strItrEnd);
			if (startChar <= endChar)
			{
				mSubsetVec.Set(startChar, endChar);
				pairIndex++;
				continue;
			}
			if (endChar)
			{
				mError = eRangeDescendingErr;
				if (mErrorStr)
				{
					char errorBuff[512];
					mErrorStr->assign(errorBuff, snprintf(errorBuff, 512,
						"Subset range %d is invalid, range start is greater than its end.", pairIndex));
				}
			} else
			{
				mError = eIncompleteRangeErr;
				if (mErrorStr)
				{
					mErrorStr->assign("Subset ranges invalid, there must be an even number of characters in the subset string.");
				}
			}
			break;
		}
		mIndexVecItr.SetIndexVec(&mSubsetVec);
		mIsValid = mError == eSubsetNoErr;
	} else if (mErrorStr)
	{
		mErrorStr->assign("Empty subset string.");
	}
}

/*************************** InitializeWithText *******************************/
/*
*	Initialize the iterator with a block of random text.  Each charcode is
*	individually added to the mSubsetVec
*/
void SubsetCharcodeIterator::InitializeWithText(
	const char*	inText)
{
	if (inText)
	{
		std::string	text(inText);
		if (text.length())
		{
			mError = eSubsetNoErr;
			mIsValid = true;
			mSubsetVec.Clear();
			std::string::const_iterator	strItr = text.begin();
			std::string::const_iterator	strItrEnd = text.end();
			uint32_t	thisChar = 0;

			for (thisChar = NextChar(strItr, strItrEnd); thisChar;
									thisChar = NextChar(strItr, strItrEnd))
			{
				if (thisChar >= 0x20)
				{
					mSubsetVec.Set(thisChar, thisChar);
				}
			}
			mIndexVecItr.SetIndexVec(&mSubsetVec);
		}
	}
}

/******************************** GetSubset ***********************************/
/*
*	Reconstitute the sorted and condensed subset string.
*/
std::string& SubsetCharcodeIterator::GetSubset(
	std::string&	outSubsetStr) const
{
	{
		Runs::const_iterator	itr = mSubsetVec.GetRuns().begin();
		Runs::const_iterator	itrEnd = mSubsetVec.GetRuns().end();

		if (mSubsetVec.GetFirstRunValue() == 0)
		{
			++itr;
		}
		while (itr != itrEnd)
		{
			AppendUTF16(*(itr++), outSubsetStr);
			AppendUTF16(*(itr++)-1, outSubsetStr);
		}
	}
	return(outSubsetStr);
}

/********************************** Next **************************************/
uint32_t SubsetCharcodeIterator::Next(void)
{
	if (mIsValid)
	{
		mIsValid = mIndexVecItr.Next() != IndexVecIterator::end;
	}
	return((uint32_t)mIndexVecItr.Current());
}

/******************************** MoveToStart *********************************/
size_t SubsetCharcodeIterator::MoveToStart(void)
{
	size_t	current = mIndexVecItr.MoveToStart();
	mIsValid = current != IndexVecIterator::end;
	return(current);
}

/********************************* MoveToEnd *********************************/
size_t SubsetCharcodeIterator::MoveToEnd(void)
{
	mIsValid = false;
	return(mIndexVecItr.MoveToEnd());
}

/***************************** GetNumCharCodes ********************************/
uint32_t SubsetCharcodeIterator::GetNumCharCodes(void) const
{
	return((uint32_t)mSubsetVec.GetCount());
}

/***************************** GetNumRuns ********************************/
uint32_t SubsetCharcodeIterator::GetNumRuns(void) const
{
	return((uint32_t)mSubsetVec.GetRuns().size()/2);
}

/********************************** NextChar **********************************/
uint32_t SubsetCharcodeIterator::NextChar(
	std::string::const_iterator&	inSubsetStrItr,
	std::string::const_iterator&	inSubsetStrItrEnd)
{
	uint32_t	nextChar = 0;
	if (inSubsetStrItr != inSubsetStrItrEnd)
	{
		uint32_t  codepoint = (uint32_t)*(inSubsetStrItr++);
		if ((codepoint & 0x80) == 0)
		{
			nextChar = codepoint;
		// 2 byte sequence
		} else if ((codepoint & 0xE0) == 0xC0)
		{
			nextChar = (uint32_t)(((codepoint & 0x1F) << 6) +
							((uint32_t)(*(inSubsetStrItr++)) & 0x3F));
		// 3 byte sequence
		} else if ((codepoint & 0xF0) == 0xE0)
		{
			nextChar = (uint32_t)(((codepoint & 0x0F) << 0xC) +
							(((uint32_t)(inSubsetStrItr[0]) & 0x3F) << 6) +
								((uint32_t)(inSubsetStrItr[1]) & 0x3F));
			inSubsetStrItr+=2;
		// 4 byte sequence
		} else if ((codepoint & 0xF8) == 0xF0)
		{
			nextChar = ((codepoint & 7) << 0x12) +
							(((uint32_t)(inSubsetStrItr[0]) & 0x3F) << 0xC) +
								(((uint32_t)(inSubsetStrItr[1]) & 0x3F) << 6) +
									((uint32_t)(inSubsetStrItr[2]) & 0x3F);
			inSubsetStrItr+=3;
		}
		
			/*
			*	No need to support 5 & 6 byte sequences.
			*	The largest Unicode 6 code point is 10FFFF, (a 4 byte sequence)
			* 5 byte sequence
			*} else if ((codepoint & 0xFC) == 0xF8)
			*{
			*	nextChar = ((codepoint & 3) << 0x18) +
			*					(((uint32_t)(inSubsetStrItr[0]) & 0x3F) << 0x12) +
			*						(((uint32_t)(inSubsetStrItr[1]) & 0x3F) << 0xC) +
			*							(((uint32_t)(inSubsetStrItr[2]) & 0x3F) << 6) +
			*								((uint32_t)(inSubsetStrItr[3]) & 0x3F);
			*	inSubsetStrItr+=4;
			*}
			* 6 byte sequence
			*} else if ((codepoint & 0xFE) == 0xFC)
			*{
			*	nextChar = ((codepoint & 1) << 0x1E) +
			*					(((uint32_t)(inSubsetStrItr[0]) & 0x3F) << 0x18) +
			*						(((uint32_t)(inSubsetStrItr[1]) & 0x3F) << 0x12) +
			*							(((uint32_t)(inSubsetStrItr[2]) & 0x3F) << 0xC) +
			*								(((uint32_t)(inSubsetStrItr[3]) & 0x3F) << 6) +
			*									((uint32_t)(inSubsetStrItr[4]) & 0x3F);
			*	inSubsetStrItr+=5;
			*}
			*
			*	For uint16_t
			*if (codepoint > 0xFFFF)
			*{
			*	nextChar = (uint32_t)(0xD7C0 + (codepoint >> 0xA)); //(>>10 in decimal) compiler expects a Hex (not Decimal) number
			*	mNextWChar = (uint32_t)(0xDC00 + (codepoint & 0x3FF));
			*} else
			*{
			*	nextChar = (uint32_t)codepoint;
			*}
			*/
	}
	return(nextChar);
}

/******************************** AppendUTF16 *********************************/
void SubsetCharcodeIterator::AppendUTF16(
	uint16_t		inCodepoint,
	std::string&	outUTF8)
{
		if (inCodepoint < 0x80)
		{
			outUTF8 += (char)inCodepoint;
		} else if (inCodepoint < 0x800)
		{
			outUTF8 += (0xC0 | inCodepoint >> 6);
			outUTF8 += (0x80 | (inCodepoint & 0x3F));
		} else/* if (inCodepoint < 0x10000)*/
		{
			outUTF8 += (0xE0 | inCodepoint >> 0xC);
			outUTF8 += (0x80 | (inCodepoint >> 6 & 0x3F));
			outUTF8 += (0x80 | (inCodepoint & 0x3F));
		}
		/*else if (inCodepoint < 0x200000)
		{
			outUTF8 += (0xF0 | inCodepoint >> 0x12);
			outUTF8 += (0x80 | inCodepoint >> 0xC & 0x3F);
			outUTF8 += (0x80 | inCodepoint >> 6 & 0x3F);
			outUTF8 += (0x80 | inCodepoint & 0x3F);
		}*/
}
