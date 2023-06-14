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
*
*	This code was ported from my FreeTypeTest project (2015).  Major changes
*	were made to implement as much code in standard C++ as possible, and use
*	smaller export data sizes to save on space.
*/


#ifndef SubsetFontCreator_h
#define SubsetFontCreator_h

#include <string>
#include "IndexVec.h"
#include "XFontGlyph.h"

enum ESubsetErr
{
	eSubsetNoErr,
	eEmptySubsetErr,
	eIncompleteRangeErr,
	eRangeDescendingErr,
	eDataOffsetTooLargeErr,
	eUnexpectedEndOfRunErr,
	eFTSetCharSizeFailedErr,
	eFTLoadCharFailedErr,
	eFTNewFaceFailedErr,
	eFTInitFreeTypeFailedErr
};

class SubsetCharcodeIterator;

class SubsetFontCreator
{
public:	
	enum EFormat
	{
		eC_HeaderFormat,
		eBinaryXfntFormat
	};
	enum EOptionsMask
	{
		e8BitsPerPixel			= 0,	// Grayscale for RGB antialiasing
		e1BitPerPixel			= 1,	// Else 8 bit grayscale
		eRotated				= 2,
		eHorizontal				= 4,	// See CreateRotatedData
		eGlyphDataSeparately	= 8,
		/*
		*	32 bit data offsets are only supported in the binary format.
		*	The SubsetFontCreator application no longer exposes the flag to
		*	create 32 bit offsets.  If this flag is exposed then attempting to
		*	create a C file header will produce garbage.
		*/
		e32BitDataOffsets		= 0x10,
		/*
		*	Minimize Height trims the overall height of the font to min y +
		*	tallest glyph.  This is useful for very large fonts.
		*/
		eMinimizeHeight			= 0x20
	};
	static int				CreateXfntFile(
								const char*				inFontFilePath,
								FILE*					inExportFile,
								FILE*					inGlyphDataExportFile,
								int32_t					inPointSize,
								int						inOptions,
								SubsetCharcodeIterator&	inCharcodeItr,
								long					inFontFaceIndex,
								const char*				inSupplementalFontFilePath,
								long					inSupplementalFontFaceIndex,
								std::string*			outErrorStr = NULL,
								std::string*			outWarningStr = NULL,
								std::string*			outInfoStr = NULL);
	static int				CreateFile(
								EFormat					inExportFormat,
								const char*				inFontFilePath,
								const char*				inExportPath,
								int32_t					inPointSize,
								int						inOptions,
								const char*				inSubset,
								long					inFontFaceIndex,
								const char*				inSupplementalFontFilePath,
								long					inSupplementalFontFaceIndex,
								std::string*			outErrorStr = NULL,
								std::string*			outWarningStr = NULL,
								std::string*			outInfoStr = NULL);
	static int				GetFaceNames(
								const char*				inFontFilePath,
								std::vector<std::string>&	outFaceNames);

protected:
	static int				XFntToC_Header(
								FILE*					inXFntFile,
								const char*				inExportPath,
								FILE*					inOutputFile,
								std::string&			inSubsetStr,
								bool					inIncludeGlyphData,
								int						inOptions);
	static uint8_t* 		CreateRotatedData(
								const uint8_t*			inBitmap,
								int						inRows,
								int						inColumns,
								int						inYOffset,
								bool					inMSBTop,
								bool					inHorizontal,
								size_t&					outDataLen);
	static int				PreviewFont(
								const char*				inFontFilePath,
								int32_t					inPointSize,
								int						inOptions,
								SubsetCharcodeIterator&	inCharcodeItr,
								long					inFontFaceIndex,
								const char*				inSupplementalFontFilePath,
								long					inSupplementalFontFaceIndex,
								FontHeader&				outFontHeader,
								GlyphHeader&			outGlyphHeader,
								std::string*			outErrorStr);
};

class SubsetCharcodeIterator
{
public:
							SubsetCharcodeIterator(
								const char*				inSubset = NULL,
								std::string*			outErrorStr = NULL);
	void					Initialize(
								const char*				inSubset);
	void					InitializeWithText(
								const char*				inText);
	bool					IsValid(void)
								{return(mIsValid);}
	uint32_t				Current(void) const
								{return((uint32_t)mIndexVecItr.Current());}
	uint32_t				GetNumCharCodes(void) const;
	const IndexVec&			GetIndexVec(void) const
								{return(mSubsetVec);}
	uint32_t				GetNumRuns(void) const;
	uint32_t				Next(void);
	size_t					MoveToStart(void);	// Rewind
	size_t					MoveToEnd(void);	// Halt iteration
	ESubsetErr				GetError(void)
								{return(mError);}
	std::string&			GetSubset(
								std::string&			outSubsetStr) const;
protected:
	bool			mIsValid;
	IndexVec		mSubsetVec;	// Consolidated and sorted subset ranges
	IndexVecIterator mIndexVecItr;
	std::string*	mErrorStr;
	ESubsetErr		mError;

							/*
							*	Only used during initialization of the subset index vec
							*/
	static uint32_t			NextChar(
								std::string::const_iterator&	inSubsetStrItr,
								std::string::const_iterator&	inSubsetStrItrEnd);
	static void				AppendUTF16(
								uint16_t				inCodepoint,
								std::string&			outUTF8);
};


#endif /* SubsetFontCreator_h */
