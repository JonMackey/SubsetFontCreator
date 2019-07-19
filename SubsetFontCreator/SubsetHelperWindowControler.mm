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
*	SubsetHelperWindowControler
*	
*	Created by Jon Mackey on 4/26/19.
*	Copyright © 2019 Jon. All rights reserved.
*/


#import "SubsetHelperWindowControler.h"
#include "SubsetFontCreator.h"

@interface SubsetHelperWindowControler ()

@end

@implementation SubsetHelperWindowControler

/****************************** windowDidLoad *********************************/
- (void)windowDidLoad
{
    [super windowDidLoad];
	charcodeTextView.delegate = self;
	
	[self initSubset];
}

/****************************** textDidChange *********************************/
- (void)textDidChange:(NSNotification *)notification
{
	if ([notification.object class] == NSTextView.class)
	{
		subsetTextField.stringValue = self.subset;
	}
}

/*********************************** ok ***************************************/
- (IBAction)ok:(id)sender
{
	[[NSApplication sharedApplication] stopModalWithCode:NSModalResponseOK];
}

/********************************* cancel *************************************/
- (IBAction)cancel:(id)sender
{
	[[NSApplication sharedApplication] stopModalWithCode:NSModalResponseCancel];
}

/****************************** subset getter *********************************/
- (NSString*)subset
{
	// Assumed to be called after the window loads
	SubsetCharcodeIterator charcodeIterator;
	charcodeIterator.InitializeWithText(charcodeTextView.string.UTF8String);
	std::string	subsetStr;
	charcodeIterator.GetSubset(subsetStr);
	return([NSString stringWithUTF8String:subsetStr.c_str()]);
}

/****************************** subset setter *********************************/
- (void)setSubset:(NSString*)inSubset
{
	// This may be called before the window loads so just copy it for now
	// subsetStr gets used in windowDidLoad
	self.subsetStr = inSubset;
	if (self.windowLoaded)
	{
		[self initSubset];
	}
}

/******************************* initSubset ***********************************/
-(void)initSubset
{
	if (_subsetStr.length)
	{
		SubsetCharcodeIterator charcodeIterator(_subsetStr.UTF8String);
		if (charcodeIterator.IsValid())
		{
			uint32_t	numCharcodes = charcodeIterator.GetNumCharCodes();
			unichar*	subsetCharcodes = new unichar[numCharcodes+1];
			unichar*	charcodes = subsetCharcodes;
			*(charcodes++) = charcodeIterator.Current();
			while (charcodeIterator.IsValid())
			{
				*(charcodes++) = charcodeIterator.Next();
			}
			NSString*	subsetStr = [NSString stringWithCharacters:subsetCharcodes length:numCharcodes];
			[charcodeTextView setString:subsetStr];
			delete [] subsetCharcodes;
			subsetTextField.stringValue = self.subset;
		}
	}
}

@end
