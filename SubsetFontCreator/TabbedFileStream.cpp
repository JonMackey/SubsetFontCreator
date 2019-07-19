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
*	TabbedFileStream
*	
*	Created by Jon Mackey on 4/13/19.
*	Copyright © 2019 Jon Mackey. All rights reserved.
*/


#include "TabbedFileStream.h"
#include <string>

/*********************************** TabbedFileStream *************************************/
/*
*	Pass the maximum number of tabs this class will return by Get()
*	inStream is the stream associated with this tab set.
*	inRetain means you want the streams ref count bumped
*/
TabbedFileStream::TabbedFileStream(
	long	inMaxTabs,
	FILE*	inStream)
	: Tabs(inMaxTabs), mStream(inStream)
{
}

/*********************************** ~TabbedFileStream ************************************/
TabbedFileStream::~TabbedFileStream(void)
{
}

/********************************* GetStream **************************************/
FILE* TabbedFileStream::GetStream(void) const
{
	return(mStream);
}

/************************************* Write **************************************/
void TabbedFileStream::Write(
	const char*	inFormat, ...)
{
	va_list argList;
	va_start(argList, inFormat);
	WriteWithArgs(inFormat, argList);
	va_end(argList);
}

/************************************* WriteWithArgs **************************************/
/*
*	Takes the same params as printf and sends the result to inStream.
*
*	This routine automatically inserts N tabs specified by "this" after
*	every newline.
*
*	This implements a new fprintf format %type called a "tab stop".
*	The %type is "t | T" of the form: %[tab stop]type.
*	The correct number of tabs will be inserted into the stream to bring the text
*	to the desired tab stop.
*	If the stream is already past this tab stop the tab stop is ignored.
*	t = tab stop after ignoring the tabs autmatically inserted after every newline.
*	T = tab stop from start of line including tabs autmatically inserted after every newline.
*
*	If this some other format type other than a tab stop, the format is accumulated
*	and passed to vfprintf to process.
*/
void TabbedFileStream::WriteWithArgs(
	const char*	inFormat,
	va_list		inArgList)
{
	if (mStream)
	{
		const uint8_t*	substringStart = (const uint8_t*)inFormat;
		const uint8_t*	thisCharPtr = (const uint8_t*)inFormat;
		long			tabPos = 0;
		uint8_t	thisChar;
		size_t	mark = ftell(mStream);
		static const long	kTabSize = 4;

		for (thisChar = *thisCharPtr; thisChar != 0;)
		{
			switch (thisChar)
			{
				case '\n':
				{
					fwrite(substringStart, thisCharPtr-substringStart, 1, mStream);
					/*
					*	Insert the platform eol terminator and
					*	insert tabs at the start of the new line and
					*	reset the tabPos counter
					*/
					#ifdef _WIN64
					static const uint8_t	kCRNL[] = "\r\n";
					fwrite(kCRNL, 2, 1, mStream);
					#else
					fputc('\n', mStream);
					#endif
					tabPos = 0;
					fwrite(Get(), Size(), 1, mStream);
					mark = ftell(mStream);
					thisChar = *++thisCharPtr;
					substringStart = thisCharPtr;
					continue;
				}
				case '%':
				{
					fwrite(substringStart, thisCharPtr-substringStart, 1, mStream);
					/*
					*	This is the heart of this routine.  This implements the tab stop
					*	type "t | T" of the form: %[tab stop][t | T]
					*	The correct number of tabs will be inserted into the stream
					*	to bring the text to [tab stop] and the tabPos var will
					*	be incremented by [tab stop].  If the stream is already past this tab stop
					*	the %T or %t is ignored.
					*	t = relative tab stop after ignoring the tabs inserted at the line start.
					*	T = absolute tab stop from start of line including tabs automatically inserted.
					*	If this some other format type other than tab position is encountered, the
					*	format is accumulated and passed to vsnprintf to process.
					*/
					const char*	formatStart = (char*)thisCharPtr;
					// Skip flags
					for (thisChar = *++thisCharPtr; thisChar != 0; thisChar = *++thisCharPtr)
					{
						switch (thisChar)
						{
							case '-':
							case '+':
							case '#':
							case ' ':
								continue;
							default:
								break;
						}
						break;
					}
					// Get the field width aka desiredTabStop
					long	width = 0L;
					/*
					*	If the width is from the arg list THEN
					*	skip the width, arg list width is not supported for tab position
					*/
					if (thisChar == '*')
					{
						thisChar = *++thisCharPtr;
					} else
					{
						for (; thisChar >= '0' && thisChar <= '9'; thisChar = *++thisCharPtr)
						{
							width = (width*10)+(thisChar-'0');
						}
					}
					/*
					*	If precision is specified THEN
					*	skip the precision, it has no meaning for a tab stop
					*/
					if (thisChar == '.')
					{
						thisChar = *++thisCharPtr;
						if (thisChar == '*')
						{
							thisChar = *++thisCharPtr;
						} else
						{
							for (; thisChar >= '0' && thisChar <= '9'; thisChar = *++thisCharPtr){}
						}
					}
					uint8_t	modifier = 0;
					//	Skip the modifiers, they have no meaning for a tab stop
					for (; thisChar != 0; thisChar = *++thisCharPtr)
					{
						switch (thisChar)
						{
							case 'h':
							case 'I':
							case 'L':
								modifier = thisChar;
								continue;
							default:
								break;
						}
						break;
					}
					long	startingTabPosition = tabPos;
					switch (thisChar)
					{
						case 'T':
							startingTabPosition += Size();
						case 't':
						{
							size_t	charsWritten = ftell(mStream);
							charsWritten -= mark;
							long	tabsToAdd = width - startingTabPosition - (charsWritten/kTabSize);
							if (tabsToAdd > 0)
							{
								fwrite(Get(tabsToAdd), tabsToAdd, 1, mStream);
								mark = ftell(mStream);
								tabPos = width;
							}
							thisChar = *++thisCharPtr;
							break;
						}
						default:
							//uint8_t typeChar = thisChar;
							thisChar = *++thisCharPtr;
							char format[20];
							long	formatLen = thisCharPtr-(uint8_t*)formatStart;
							if (formatLen < 20)
							{
								memcpy(format, formatStart, formatLen);
								format[formatLen] = 0;
								vfprintf(mStream, format, inArgList);
							#if 0
								/*
								*	Based on the format type character, advance the argList.
								*	Used if vfprintf doesn't do this.
								*/
								switch (typeChar)
								{
									case 'c':
									#ifdef __GNUC__
										va_arg(inArgList, long);
									#else
										va_arg(inArgList, char);
									#endif
										break;
									case 'X':
									case 'd':
									case 'i':
									case 'o':
									case 'u':
									case 'x':
										switch (modifier)
										{
											case 'l':	// There's also 'll' but???
												va_arg(inArgList, long);
												break;
											case 'h':
											#ifdef __GNUC__
												va_arg(inArgList, long);
											#else
												va_arg(inArgList, short);
											#endif
												break;
											case 'L':
												va_arg(inArgList, long long);
												break;
											default:
												va_arg(inArgList, int);
												break;
										}
										break;
									case 'E':
									case 'G':
									case 'e':
									case 'f':
									case 'g':
										switch (modifier)
										{
										//	case 'l':	// error
										//	case 'h':	// error
											case 'L':
												va_arg(inArgList, long double);
												break;
											default:
												va_arg(inArgList, double);
												break;
										}
										break;
									case 'n':
									case 'p':
									case 's':
										va_arg(inArgList, char*); 	// sizeof any pointer
										break;
								}
						#endif
							}
					}
					substringStart = thisCharPtr;
					continue;
				}
			}
			thisChar = *++thisCharPtr;
		}
		if ((thisCharPtr-substringStart) > 0L)
		{
			fwrite(substringStart, thisCharPtr-substringStart, 1, mStream);
		}
	}
}
