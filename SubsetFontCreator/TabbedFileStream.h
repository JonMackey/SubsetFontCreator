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


#ifndef TabbedFileStream_h
#define TabbedFileStream_h

#include <stdio.h>

#include <stdarg.h>
#include "Tabs.h"

class TabbedFileStream : public Tabs
{
public:
							TabbedFileStream(
								long					inMaxTabs,
								FILE*					inStream);
	virtual					~TabbedFileStream(void);
	FILE*					GetStream(void) const;
	bool					StreamIsValid(void) const
								{return(mStream != 0);}
	void					Write(
								const char*				inFormat, ...);
	void					WriteWithArgs(
								const char*				inFormat,
								va_list					inArgList);
protected:
	FILE*		mStream;
};
#endif /* TabbedFileStream_h */
