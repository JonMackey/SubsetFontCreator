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
*	AppDelegate
*	
*	Created by Jon Mackey on 12/11/18.
*	Copyright © 2018 Jon. All rights reserved.
*/


#import "AppDelegate.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

/*********************** applicationDidFinishLaunching ************************/
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	self.mainWindowController = [MainWindowController alloc];
	[[self.mainWindowController initWithWindowNibName:@"MainWindowController"].window makeKeyAndOrderFront:nil];
}

/******************************** openURLs ************************************/
- (void)application:(NSApplication *)inApplication openURLs:(NSArray<NSURL *> *)inUrls
{
	if (inUrls.count == 1)
	{
		[_mainWindowController doOpen:inUrls[0]];
	}
}

/************************** applicationWillTerminate **************************/
- (void)applicationWillTerminate:(NSNotification *)aNotification
{

}


@end
