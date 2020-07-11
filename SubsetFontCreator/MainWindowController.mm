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
*	MainWindowController
*	
*	Created by Jon Mackey on 12/11/18.
*	Copyright © 2018 Jon MAckey. All rights reserved.
*/


#import "MainWindowController.h"
#import "NSTextButtonCell.h"
#include <string>
#include <ft2build.h>
#include "XFontGlyph.h"
#include "SubsetFontCreator.h"
#include FT_FREETYPE_H
#ifdef DEBUG
extern "C" {
	int z_verbose = 0;
	void z_error(char *m)
	{
		fprintf(stderr, "%s\n", m);
	}
}
#endif
@interface MainWindowController ()

@end
#define SANDBOX_ENABLED 1
@implementation MainWindowController
#if SANDBOX_ENABLED
NSString *const kSavedSetsURLBMKey = @"savedSetsURLBM";
NSString *const kFontURLBMKey = @"fontURLBM";
NSString *const kSupplementalFontURLBMKey = @"supplementalFontURLBM";
#else
NSString *const kSavedSetsPathKey = @"savedSetsPath";
NSString *const kFontPathKey = @"fontPath";
NSString *const kSupplementalFontPathKey = @"supplementalFontPath";
#endif

NSString *const kSubsetFileIdentifierKey = @"subsetFileID";
NSString *const kSubsetFileIdentifier = @"1521E46C-08C5-4276-913C-2FF08F4DCB1C";
NSString *const kFontFaceKey = @"fontFace";
NSString *const kSupplementalFontFaceKey = @"supplementalFontFace";
NSString *const kFontSizeKey = @"fontSize";
NSString *const kIs1BitKey = @"is1Bit";
NSString *const kRotateKey = @"rotate";
NSString *const kHorizontalKey = @"horizontal";	// 0 = vertical, 1 = horizontal
NSString *const kSubsetStrKey = @"subset";
NSString *const kSampleStrKey = @"sample";
NSString *const kSampleTextBGColorStrKey = @"sampleTextBGColor";
NSString *const kSampleTextColorStrKey = @"sampleTextColor";

NSString *const kLastExportTypeKey = @"lastExportType";
NSString *const kLastExportOptionsKey = @"lastExportOptions";

NSString *const k32BitDataOffsetsKey = @"32BitDataOffsets";
NSString *const kSampleDisplayKey = @"sampleDisplay";

NSString *const kSelectPrompt = @"Select";

struct SMenuItemDesc
{
	NSInteger	mainMenuTag;
	NSInteger	subMenuTag;
    SEL action;
};

SMenuItemDesc	menuItems[] = {
	{1,10, @selector(open:)},
	{1,13, @selector(save:)},
	{1,14, @selector(saveas:)}
};

/******************************* windowDidLoad ********************************/
- (void)windowDidLoad
{
    [super windowDidLoad];

	{
		const SMenuItemDesc*	miDesc = menuItems;
		const SMenuItemDesc*	miDescEnd = &menuItems[sizeof(menuItems)/sizeof(SMenuItemDesc)];
		for (; miDesc < miDescEnd; miDesc++)
		{
			NSMenuItem *menuItem = [[[NSApplication sharedApplication].mainMenu itemWithTag:miDesc->mainMenuTag].submenu itemWithTag:miDesc->subMenuTag];
			if (menuItem)
			{
				// Assign this object as the target.
				menuItem.target = self;
				menuItem.action = miDesc->action;
			}
		}
	}

	if (self.logViewController == nil)
	{
		_logViewController = [[LogViewController alloc] initWithNibName:@"LogViewController" bundle:nil];
		// embed the current view to our host view
		[serialView addSubview:[self.logViewController view]];

		// make sure we automatically resize the controller's view to the current window size
		[[self.logViewController view] setFrame:[serialView bounds]];
	}
	
	if (self.formatViewController == nil)
	{
		_formatViewController  = [[NSViewController alloc] initWithNibName:@"XfntExportFormat" bundle:nil];
		NSPopUpButton* formatPopupBtn = [_formatViewController.view viewWithTag:2];
		formatPopupBtn.action = @selector(changeFormat:);
		formatPopupBtn.target = self;
	}
	
	if (sampleDisplayPopupButton)
	{
		NSURL*	arduinoDisplaysURL = [[NSBundle mainBundle] URLForResource:@"ArduinoDisplays" withExtension:@"plist"];
		_arduinoDisplays = [NSArray arrayWithContentsOfURL:arduinoDisplaysURL error:nil];
		NSMutableArray<NSString*>* displayNames = [NSMutableArray arrayWithCapacity:_arduinoDisplays.count];
		[_arduinoDisplays enumerateObjectsUsingBlock:^(NSDictionary * inDisplayDict, NSUInteger inIndex, BOOL * outStop)
		{
			[displayNames addObject:[inDisplayDict objectForKey:@"name"]];
		}];
		[sampleDisplayPopupButton removeAllItems];
		[sampleDisplayPopupButton addItemsWithTitles:displayNames];
		NSNumber* savedSampleDisplayIndex = [[NSUserDefaults standardUserDefaults] objectForKey:kSampleDisplayKey];
		[sampleDisplayPopupButton selectItemAtIndex:savedSampleDisplayIndex.integerValue];
		[self setDisplay:sampleDisplayPopupButton];
	}
	
	/*
	*	Automatically open the previous saved set, if any
	*/
	{
		NSURL*	docURL = nil;
	#if SANDBOX_ENABLED
		NSData* savedSetsBM = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsURLBMKey];
		if (savedSetsBM)
		{
			docURL = [NSURL URLByResolvingBookmarkData: savedSetsBM
								options:NSURLBookmarkResolutionWithoutUI+NSURLBookmarkResolutionWithoutMounting+NSURLBookmarkResolutionWithSecurityScope
									relativeToURL:NULL bookmarkDataIsStale:NULL error:NULL];
		}
	#else
		NSString* savedSetPath = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsPathKey];
		docURL = [NSURL fileURLWithPath:savedSetPath isDirectory:NO];
	#endif
		if (docURL)
		{
			[self doOpen:docURL];
		}
	}
	/*
	*	Get the saved sample background and text colors.
	*/
	{
		NSData* colorData = [[NSUserDefaults standardUserDefaults] dataForKey:kSampleTextColorStrKey];
		if (colorData != nil)
		{
			((NSTextButtonCell*)textColorButton.cell).fillColor = (NSColor *)[NSKeyedUnarchiver unarchivedObjectOfClass:[NSColor class] fromData:colorData error:nil];
		} else
		{
			((NSTextButtonCell*)textColorButton.cell).fillColor = NSColor.whiteColor;
		}
		textColorButton.needsDisplay = YES;
		colorData = [[NSUserDefaults standardUserDefaults] dataForKey:kSampleTextBGColorStrKey];
		if (colorData != nil)
		{
			((NSTextButtonCell*)textBGColorButton.cell).fillColor = (NSColor *)[NSKeyedUnarchiver unarchivedObjectOfClass: [NSColor class] fromData:colorData error:nil];
		} else
		{
			((NSTextButtonCell*)textBGColorButton.cell).fillColor = NSColor.blackColor;
		}
		[self updateTextBGColorTextColor];
	}
	offsetWidth16Radio.state = NSControlStateValueOn;
}

/****************************** validateMenuItem ******************************/
- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	BOOL	isValid = YES;
	switch (menuItem.tag)
	{
		case 13:	// Save Set…
		case 14:	// Save Set As…
		case 15:	// Export…
		{
			BOOL isDirectory = YES;
			isValid = [[NSFileManager defaultManager] fileExistsAtPath:fontPathControl.URL.path isDirectory:&isDirectory] && isDirectory == NO;
			break;
		}
	}
	return(isValid);
}

#pragma mark - Path Popup support
/**************************** willDisplayOpenPanel ****************************/
- (void)pathControl:(NSPathControl *)pathControl willDisplayOpenPanel:(NSOpenPanel *)openPanel
{
	if (pathControl.tag == 1 ||
		pathControl.tag == 10)
	{
		NSURL*	folderURL = [pathControl URL];
		[openPanel setCanChooseDirectories:NO];
		[openPanel setCanChooseFiles:YES];
		[openPanel setAllowsMultipleSelection:NO];
		openPanel.allowedFileTypes = @[@"otf", @"ttf", @"ttc"];
		openPanel.directoryURL = folderURL;
		openPanel.message = @"Select a font";
		openPanel.prompt = kSelectPrompt;
	}
}

/****************************** fontPathChanged *******************************/
- (IBAction)fontPathChanged:(id)sender
{
	[self loadFacePopup:facePopupButton fontURL:fontPathControl.URL faceIndex:0];
}

/************************ supplementalFontPathChanged *************************/
- (IBAction)supplementalFontPathChanged:(id)sender
{
	[self loadFacePopup:suppFacePopupButton fontURL:suppFontPathControl.URL faceIndex:0];
}

/******************************* loadFacePopup ********************************/
-(void)loadFacePopup:(NSPopUpButton*)inFacePopup fontURL:(NSURL*)inFontURL faceIndex:(NSInteger)inFaceIndex
{
	std::vector<std::string>	fontFaces;
	SubsetFontCreator::GetFaceNames(inFontURL.path.UTF8String, fontFaces);
	[inFacePopup removeAllItems];
	std::vector<std::string>::iterator	itr = fontFaces.begin();
	std::vector<std::string>::iterator	itrEnd = fontFaces.end();
	for (; itr != itrEnd; ++itr)
	{
		[inFacePopup addItemWithTitle:[NSString stringWithUTF8String:itr->c_str()]];
	}
	if (inFaceIndex <= fontFaces.size())
	{
		[inFacePopup selectItemAtIndex:inFaceIndex];
	}
}

#if 0
/******************************* willPopUpMenu ********************************/
- (void)pathControl:(NSPathControl *)pathControl willPopUpMenu:(NSMenu *)menu
{
	if (pathControl.tag == 1)
	{
		NSMenuItem *openTemporaryFolderMenuItem = [[NSMenuItem alloc]initWithTitle:@"Show in Finder" action:@selector(showInFinder:) keyEquivalent:[NSString string]];
		openTemporaryFolderMenuItem.target = self;
		openTemporaryFolderMenuItem.tag = pathControl.tag;
		[menu insertItem:[NSMenuItem separatorItem] atIndex:0];
		[menu insertItem:openTemporaryFolderMenuItem atIndex:0];
	}
}

/******************************* showInFinder *********************************/
- (IBAction)showInFinder:(id)sender
{
	NSURL*	folderURL = nil;
	switch (((NSMenuItem*)sender).tag)
	{
		case 1:
			folderURL = [fontPathControl URL];
			break;
		case 10:
			folderURL = [suppFontPathControl URL];
			break;
	}
	if (folderURL)
	{
	#if SANDBOX_ENABLED
		if ([folderURL startAccessingSecurityScopedResource])
		{
			[[NSWorkspace sharedWorkspace] openURL:folderURL];
			[folderURL stopAccessingSecurityScopedResource];
		}
	#else
		[[NSWorkspace sharedWorkspace] openURL:folderURL];
	#endif
	}
}
#endif

/******************************* setDisplay ***********************************/
- (IBAction)setDisplay:(NSPopUpButton*)sender
{
	[arduinoDisplayView setDisplay:[_arduinoDisplays objectAtIndex: sender.indexOfSelectedItem]];
}

/******************************* showHelper ***********************************/
- (IBAction)showHelper:(id)sender
{
	SubsetHelperWindowControler* subsetHelperWindowController = [[SubsetHelperWindowControler alloc] initWithWindowNibName:@"SubsetHelper"];
	
	subsetHelperWindowController.subset = subsetTextField.stringValue;
	if ([[NSApplication sharedApplication] runModalForWindow:subsetHelperWindowController.window] == NSModalResponseOK)
	{
		subsetTextField.stringValue = subsetHelperWindowController.subset;
	}
	[subsetHelperWindowController.window close];
}

/************************** setOffsetWidthRadios ******************************/
- (IBAction)setOffsetWidthRadios:(NSButton*)sender
{
}

/********************************** open **************************************/
- (IBAction)open:(id)sender
{
	// ToDo: add check for dirty of current set, if dirty ask user if save is needed first.
	NSURL*	baseURL = NULL;
#if SANDBOX_ENABLED
	NSData* savedSetsBM = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsURLBMKey];
	if (savedSetsBM)
	{
		baseURL = [NSURL URLByResolvingBookmarkData:
	 					savedSetsBM
	 						options:NSURLBookmarkResolutionWithoutUI+NSURLBookmarkResolutionWithoutMounting+NSURLBookmarkResolutionWithSecurityScope
	 							relativeToURL:NULL bookmarkDataIsStale:NULL error:NULL];
		baseURL = [NSURL fileURLWithPath:[[baseURL path] stringByDeletingLastPathComponent] isDirectory:YES];
	}
#else
	NSString* savedSetsPath = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsPathKey];
	baseURL = savedSetsPath ? [NSURL fileURLWithPath:[savedSetsPath stringByDeletingLastPathComponent] isDirectory:YES] : nil;
#endif
	NSOpenPanel*	openPanel = [NSOpenPanel openPanel];
	if (openPanel)
	{
		[openPanel setCanChooseDirectories:NO];
		[openPanel setCanChooseFiles:YES];
		[openPanel setAllowsMultipleSelection:NO];
		openPanel.directoryURL = baseURL;
		openPanel.allowedFileTypes = @[@"sfl2"];
		[openPanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result)
		{
			if (result == NSModalResponseOK)
			{
				NSArray* urls = [openPanel URLs];
				if ([urls count] == 1)
				{
					[self doOpen:urls[0]];
				}
			}
		}];
	}
}

/********************************* doOpen *************************************/
- (void)doOpen:(NSURL*)inDocURL
{
	if ([self setSetWithContentsOfURL:inDocURL])
	{
		[self newRecentSetDoc:inDocURL];
	}
}

/*********************************** save *************************************/
- (IBAction)save:(id)sender
{
#if SANDBOX_ENABLED
	NSData* savedSetsBM = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsURLBMKey];
	NSURL*	docURL = [NSURL URLByResolvingBookmarkData:
	 					savedSetsBM
	 						options:NSURLBookmarkResolutionWithoutUI+NSURLBookmarkResolutionWithoutMounting+NSURLBookmarkResolutionWithSecurityScope
	 							relativeToURL:NULL bookmarkDataIsStale:NULL error:NULL];
#else
	NSString* savedSetsPath = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsPathKey];
	NSURL*	docURL = savedSetsPath ? [NSURL fileURLWithPath:savedSetsPath isDirectory:NO] : nil;
#endif
	if (docURL)
	{
		if ([[self initialDocName] compare:[[docURL.path lastPathComponent] stringByDeletingPathExtension]] != NSOrderedSame)
		{
			NSAlert *alert = [[NSAlert alloc] init];
			[alert setMessageText:@"The font name and or data options have changed, overwrite anyway?"];
			[alert addButtonWithTitle:@"OK"];
			[alert addButtonWithTitle:@"Cancel"];
			[alert setAlertStyle:NSAlertStyleWarning];

			[alert beginSheetModalForWindow:self.window completionHandler:^(NSModalResponse returnCode)
			{
				if (returnCode == NSAlertSecondButtonReturn)
				{
        			return;
   				}
			}];
		}
		[self doSave:docURL];
	} else
	{
		[self saveas:sender];
	}
}

/******************************* initialDocName *******************************/
-(NSString*)initialDocName
{
	NSInteger	format = formatPopupButton.selectedTag;
	return([NSString stringWithFormat:@"%@_%d%s%s%s",
		[[[[fontPathControl URL] path] lastPathComponent] stringByDeletingPathExtension],
		pointSizeTextField.intValue,
		format == SubsetFontCreator::e8BitsPerPixel ? "" : "_",	// else 1 bit
		format & SubsetFontCreator::e1BitPerPixel ? "1b":"",
		format & SubsetFontCreator::eRotated ? "Rt":""]);
}

/********************************* saveas *************************************/
- (IBAction)saveas:(id)sender
{
#if SANDBOX_ENABLED
	NSData* savedSetsBM = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsURLBMKey];
	NSURL*	docURL = [NSURL URLByResolvingBookmarkData:
	 					savedSetsBM
	 						options:NSURLBookmarkResolutionWithoutUI+NSURLBookmarkResolutionWithoutMounting+NSURLBookmarkResolutionWithSecurityScope
	 							relativeToURL:NULL bookmarkDataIsStale:NULL error:NULL];
#else
	NSString* savedSetsPath = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsPathKey];
	NSURL*	docURL = savedSetsPath ? [NSURL fileURLWithPath:savedSetsPath isDirectory:NO] : nil;
#endif
	NSURL*	baseURL = nil;
	BOOL isDirectory = NO;
	if (docURL &&
		[[NSFileManager defaultManager] fileExistsAtPath:docURL.path isDirectory:&isDirectory])
	{
		baseURL = isDirectory ? docURL : [NSURL fileURLWithPath:[docURL.path stringByDeletingLastPathComponent] isDirectory:YES];
	}

	NSString*	initialName = [self initialDocName];

	NSSavePanel*	savePanel = [NSSavePanel savePanel];
	if (savePanel)
	{
		savePanel.directoryURL = baseURL;
		savePanel.allowedFileTypes = @[@"sfl2"];
		savePanel.nameFieldStringValue = initialName;
		[savePanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result)
		{
			if (result == NSModalResponseOK)
			{
				NSURL* docURL = savePanel.URL;
				if (docURL)
				{
					[self doSave:docURL];
				}
			}
		}];
	}
}

/********************************* doSave *************************************/
- (void)doSave:(NSURL*)inDocURL
{
	if ([self writeSetToURL:inDocURL])
	{
		[self newRecentSetDoc:inDocURL];
	}
}

/*************************** newRecentSetDoc *****************************/
- (void)newRecentSetDoc:(NSURL*)inDocURL
 {
#if SANDBOX_ENABLED
	NSError*	error;
	NSData* savedSetsBM = [inDocURL bookmarkDataWithOptions:NSURLBookmarkCreationWithSecurityScope
			includingResourceValuesForKeys:NULL relativeToURL:NULL error:&error];
	[[NSUserDefaults standardUserDefaults] setObject:savedSetsBM forKey:kSavedSetsURLBMKey];
#else
	[[NSUserDefaults standardUserDefaults] setObject:inDocURL.path forKey:kSavedSetsPathKey];
#endif
	[[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:inDocURL];
	[self.window setTitle:[inDocURL.path lastPathComponent]];
}

/*********************** writeSetToURL *************************/
- (BOOL)writeSetToURL:(NSURL*)inDocURL
{
	BOOL success = NO;
	if (inDocURL)
	{
#if SANDBOX_ENABLED
		NSError*	error;
		NSData* fontURLBM = [[fontPathControl URL] bookmarkDataWithOptions:
				NSURLBookmarkCreationWithSecurityScope |
					NSURLBookmarkCreationSecurityScopeAllowOnlyReadAccess
						includingResourceValuesForKeys:NULL relativeToURL:NULL error:&error];
		NSError*	suppError;
		NSData* supplementalFontURLBM = [[suppFontPathControl URL] bookmarkDataWithOptions:
				NSURLBookmarkCreationWithSecurityScope |
					NSURLBookmarkCreationSecurityScopeAllowOnlyReadAccess
						includingResourceValuesForKeys:NULL relativeToURL:NULL error:&suppError];
		if (error)
		{
			NSLog(@"%@\n", error);
		}
#endif
		NSInteger	format = formatPopupButton.selectedTag;
		NSDictionary* setDict = [NSDictionary dictionaryWithObjectsAndKeys:
			kSubsetFileIdentifier, kSubsetFileIdentifierKey,
			[NSNumber numberWithInteger:[pointSizeTextField integerValue]], kFontSizeKey,
			[NSNumber numberWithInteger:(format & SubsetFontCreator::e1BitPerPixel) != 0 ? 1 : 0], kIs1BitKey,
			[NSNumber numberWithInteger:(format & SubsetFontCreator::eRotated) != 0 ? 1 : 0], kRotateKey,
			[NSNumber numberWithInteger:(format & SubsetFontCreator::eHorizontal) != 0 ? 1 : 0], kHorizontalKey,
			[NSNumber numberWithInteger:[offsetWidth32Radio state]], k32BitDataOffsetsKey,
			[subsetTextField stringValue], kSubsetStrKey,
			[NSNumber numberWithInteger:facePopupButton.indexOfSelectedItem], kFontFaceKey,
			[NSNumber numberWithInteger:suppFacePopupButton.indexOfSelectedItem], kSupplementalFontFaceKey,
			/*[sampleTextField stringValue], kSampleStrKey,
			[sampleDisplayPopupButton titleOfSelectedItem], kSampleDisplayKey,*/
#if SANDBOX_ENABLED
			fontURLBM, kFontURLBMKey,
			supplementalFontURLBM, kSupplementalFontURLBMKey,
#else
			[[fontPathControl URL] path], kFontPathKey,
			[[suppFontPathControl URL] path], kSupplementalFontPathKey,
#endif
			nil];

#if SANDBOX_ENABLED
		[inDocURL startAccessingSecurityScopedResource];
		NSData* archivedData = [NSKeyedArchiver archivedDataWithRootObject:setDict requiringSecureCoding:NO error:nil];
		success = [archivedData writeToURL:inDocURL atomically:NO];
		[inDocURL stopAccessingSecurityScopedResource];
#else
		NSData* archivedData = [NSKeyedArchiver archivedDataWithRootObject:setDict requiringSecureCoding:NO error:nil];
		success = [archivedData writeToURL:inDocURL atomically:NO];
#endif
	}
	return(success);
}

/*********************** setSetWithContentsOfURL *************************/
- (BOOL)setSetWithContentsOfURL:(NSURL*)inDocURL
{
	BOOL success = NO;
	[self.logViewController clear:self];
	if (inDocURL)
	{
		NSError*	error = nil;
#if SANDBOX_ENABLED
		[inDocURL startAccessingSecurityScopedResource];
		NSData* archivedData = [NSData dataWithContentsOfURL:inDocURL];
		[inDocURL stopAccessingSecurityScopedResource];
		//NSArray* archivedSet = [NSKeyedUnarchiver unarchiveObjectWithData:archivedData]; << depreciated
		NSSet* classes = [NSSet setWithObjects:[NSDictionary class], [NSData class], [NSNumber class], [NSString class], nil];
#else
		NSData* archivedData = [NSData dataWithContentsOfURL:inDocURL];
		NSSet* classes = [NSSet setWithObjects:[NSDictionary class], [NSNumber class], [NSString class], nil];
#endif
		NSDictionary* archivedSet = [NSKeyedUnarchiver unarchivedObjectOfClasses:classes fromData:archivedData error:&error];
		if (error)
		{
			NSLog(@"%@", error);
		}
		if (archivedSet &&
			[archivedSet isKindOfClass:[NSDictionary class]] &&
			[[archivedSet objectForKey:kSubsetFileIdentifierKey] isEqualToString:kSubsetFileIdentifier])
		{
			success = YES;
			BOOL	bookmarkIsStale = NO;
#if SANDBOX_ENABLED
			NSURL*	supplementalFontURL = [NSURL URLByResolvingBookmarkData: [archivedSet objectForKey:kSupplementalFontURLBMKey]
			options:NSURLBookmarkResolutionWithoutUI+NSURLBookmarkResolutionWithoutMounting+NSURLBookmarkResolutionWithSecurityScope
					relativeToURL:NULL bookmarkDataIsStale:&bookmarkIsStale error:nil];	// error ignored
			NSURL*	fontURL = [NSURL URLByResolvingBookmarkData: [archivedSet objectForKey:kFontURLBMKey]
			options:NSURLBookmarkResolutionWithoutUI+NSURLBookmarkResolutionWithoutMounting+NSURLBookmarkResolutionWithSecurityScope
					relativeToURL:NULL bookmarkDataIsStale:&bookmarkIsStale error:&error];
#else
			NSURL*	fontURL = [NSURL fileURLWithPath:[archivedSet objectForKey:kFontPathKey]];
			NSURL*	supplementalFontURL = [NSURL fileURLWithPath:[archivedSet objectForKey:kSupplementalFontPathKey]];
#endif
			if (error)
			{
				[self.logViewController postErrorString:error.localizedDescription];
			}
			if (fontURL)
			{
				[fontPathControl setURL:fontURL];
				[self loadFacePopup:facePopupButton fontURL:fontURL faceIndex:[[archivedSet objectForKey:kFontFaceKey] integerValue]];
				if (supplementalFontURL)
				{
					[suppFontPathControl setURL:(supplementalFontURL)];
					[self loadFacePopup:suppFacePopupButton fontURL:supplementalFontURL faceIndex:[[archivedSet objectForKey:kSupplementalFontFaceKey] integerValue]];
				}
				[pointSizeTextField setIntegerValue:[[archivedSet objectForKey:kFontSizeKey] integerValue]];
				{
					// The format settings are broken out for backward compatiblity.
					// The old controls were checkboxes with corresponding keys.
					NSInteger	format = [[archivedSet objectForKey:kIs1BitKey] integerValue] ?
											SubsetFontCreator::e1BitPerPixel : SubsetFontCreator::e8BitsPerPixel;
					if ([[archivedSet objectForKey:kRotateKey] integerValue])
					{
						format |= SubsetFontCreator::eRotated;
						if ([[archivedSet objectForKey:kHorizontalKey] integerValue])
						{
							format |= SubsetFontCreator::eHorizontal;
						}
					}
					[formatPopupButton selectItemWithTag:format];
				}
				[([[archivedSet objectForKey:k32BitDataOffsetsKey] integerValue] ? offsetWidth32Radio : offsetWidth16Radio) setState:NSControlStateValueOn];
				[subsetTextField setStringValue:[archivedSet objectForKey:kSubsetStrKey]];
				/*[sampleTextField setStringValue:[archivedSet objectForKey:kSampleStrKey]];
				{
					NSString*	sampleDisplayName = [archivedSet objectForKey:kSampleDisplayKey];
					if (sampleDisplayName)
					{
						NSInteger	displayItemIndex = [sampleDisplayPopupButton indexOfItemWithTitle:sampleDisplayName];
						if (displayItemIndex >= 0)
						{
							[sampleDisplayPopupButton selectItemAtIndex:displayItemIndex];
							[self setDisplay:sampleDisplayPopupButton];
						}
					}
				}*/
			} else
			{
				success = NO;
			}
		}
	}
	return(success);
}
/******************************* exportSet ************************************/
- (IBAction)exportSet:(id)sender
{
	std::string	errorStr;
	SubsetCharcodeIterator subsetCharItr(subsetTextField.stringValue.UTF8String, &errorStr);
	if (subsetCharItr.IsValid())
	{
	#if SANDBOX_ENABLED
		NSData* savedSetsBM = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsURLBMKey];
		NSURL*	docURL = [NSURL URLByResolvingBookmarkData:
							savedSetsBM
								options:NSURLBookmarkResolutionWithoutUI+NSURLBookmarkResolutionWithoutMounting+NSURLBookmarkResolutionWithSecurityScope
									relativeToURL:NULL bookmarkDataIsStale:NULL error:NULL];
	#else
		NSString* savedSetsPath = [[NSUserDefaults standardUserDefaults] objectForKey:kSavedSetsPathKey];
		NSURL*	docURL = savedSetsPath ? [NSURL fileURLWithPath:savedSetsPath isDirectory:NO] : nil;
	#endif
		NSURL*	baseURL = docURL ? [NSURL fileURLWithPath:[docURL.path stringByDeletingLastPathComponent] isDirectory:YES] : nil;

		NSString*	initialName = [self initialDocName];
		
		_savePanel = [NSSavePanel savePanel];
		if (_savePanel)
		{
			_savePanel.directoryURL = baseURL;
			
			_savePanel.accessoryView = _formatViewController.view;
			NSString* lastExportType = [[NSUserDefaults standardUserDefaults] objectForKey:kLastExportTypeKey];
			NSNumber* lastExportOptions = [[NSUserDefaults standardUserDefaults] objectForKey:kLastExportOptionsKey];
			__block NSArray* exportTypes = @[@"h", @"xfnt"]; // << must match menu item order in popup button
			__block int options = 0;
			__block NSURL* exportURL = nil;
			NSPopUpButton* formatPopupBtn = [_formatViewController.view viewWithTag:2];
			__block NSUInteger typeIndex = [exportTypes indexOfObject:lastExportType];
			if (typeIndex < 0 || typeIndex >= formatPopupBtn.menu.numberOfItems)
			{
				typeIndex = 0;
			}
			[formatPopupBtn selectItemAtIndex:typeIndex];
			NSButton* glyphDataSeparatelyCheckBox = [_formatViewController.view viewWithTag:3];
			glyphDataSeparatelyCheckBox.state = (lastExportOptions.intValue & SubsetFontCreator::eGlyphDataSeparately) ? NSControlStateValueOn : NSControlStateValueOff;
			_savePanel.allowedFileTypes = @[[exportTypes objectAtIndex:typeIndex]];
			_savePanel.nameFieldStringValue = initialName;
			[_savePanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result)
			{
				if (result == NSModalResponseOK)
				{
					exportURL = self.savePanel.URL;
					if (exportURL)
					{
						typeIndex = formatPopupBtn.indexOfSelectedItem;
						[[NSUserDefaults standardUserDefaults] setObject:[exportTypes objectAtIndex:typeIndex] forKey:kLastExportTypeKey];
						options = glyphDataSeparatelyCheckBox.state == NSControlStateValueOn ? SubsetFontCreator::eGlyphDataSeparately : 0;
						[[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithInt:options] forKey:kLastExportOptionsKey];
						options |= (int)(self->formatPopupButton.selectedTag);
						//options |= self->offsetWidth32Radio.state == NSControlStateValueOn ? SubsetFontCreator::e32BitDataOffsets : 0;
						[self.savePanel orderOut:nil];
						if (options & SubsetFontCreator::eGlyphDataSeparately &&
							access([exportURL URLByDeletingLastPathComponent].path.UTF8String, W_OK) == -1)
						{
							NSOpenPanel*	openPanel = [NSOpenPanel openPanel];
							[openPanel setCanChooseDirectories:YES];
							[openPanel setCanChooseFiles:NO];
							[openPanel setAllowsMultipleSelection:NO];
							openPanel.showsHiddenFiles = NO;
							openPanel.directoryURL = [exportURL URLByDeletingLastPathComponent];
							openPanel.message = @"Please allow access to this folder.";
							openPanel.prompt = @"Allow";
							[openPanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result)
							{
								if (result == NSModalResponseOK)
								{
									NSArray* urls = [openPanel URLs];
									if ([urls count] == 1)
									{
										NSURL* parentFolderURL = urls[0];
										[openPanel orderOut:nil];
										if ([parentFolderURL startAccessingSecurityScopedResource])
										{
											[self doExport:exportURL format:typeIndex options:options];
											[parentFolderURL stopAccessingSecurityScopedResource];
										}
									}
								}
							}];
						} else
						{
							[self doExport:exportURL format:typeIndex options:options];
						}
					}
				}
			}];
		}
	} else
	{
		[_logViewController postErrorString:[NSString stringWithFormat:@"%s", errorStr.c_str()]];
	}
}


/****************************** changeFormat **********************************/
- (IBAction)changeFormat:(id)sender
{
	NSPopUpButton* formatPopupBtn = sender;
	NSUInteger typeIndex = formatPopupBtn.indexOfSelectedItem;
	_savePanel.allowedFileTypes = @[[@[@"h", @"xfnt"] objectAtIndex:typeIndex]];
}

/******************************** doExport ************************************/
- (void)doExport:(NSURL*)inDocURL format:(NSUInteger)inFormat options:(int)inOptions
{

	BOOL isDirectory = YES;
	if ([[NSFileManager defaultManager] fileExistsAtPath:fontPathControl.URL.path isDirectory:&isDirectory] &&
		isDirectory == NO)
	{
		if ([inDocURL startAccessingSecurityScopedResource])
		{
			[_logViewController clear:self];
			std::string	errorStr, warningStr, infoStr;
			int32_t	pointSize = (int32_t)pointSizeTextField.integerValue;
			if (pointSize < 4)
			{
				pointSize = 4;
			} else if (pointSize > 72)
			{
				pointSize = 72;
			}
			BOOL	suppFontOK = [[NSFileManager defaultManager] fileExistsAtPath:suppFontPathControl.URL.path isDirectory:&isDirectory] &&
																		isDirectory == NO;
			if (!suppFontOK)
			{
				warningStr.append("Supplemental Font not found.\n");
			}

			SubsetFontCreator::CreateFile(
				(SubsetFontCreator::EFormat)inFormat,
				fontPathControl.URL.path.UTF8String,
				inDocURL.path.UTF8String,
				pointSize,
				inOptions,
				subsetTextField.stringValue.UTF8String,
				facePopupButton.indexOfSelectedItem,
				suppFontOK ? suppFontPathControl.URL.path.UTF8String : nil,
				suppFacePopupButton.indexOfSelectedItem,
				&errorStr,
				&warningStr,
				&infoStr);
			[inDocURL stopAccessingSecurityScopedResource];
			if (errorStr.length())
			{
				[_logViewController postErrorString:[NSString stringWithUTF8String:errorStr.c_str()]];
			}
			if (warningStr.length())
			{
				[_logViewController postWarningString:[NSString stringWithUTF8String:warningStr.c_str()]];
			}
			if (infoStr.length())
			{
				[_logViewController postString:[NSString stringWithUTF8String:infoStr.c_str()]];
			}
		}
	}
}

/********************************* loadSample *********************************/
- (IBAction)loadSample:(id)sender
{
	int32_t	pointSize = (int32_t)pointSizeTextField.integerValue;
	if (pointSize < 4)
	{
		pointSize = 4;
	} else if (pointSize > 72)
	{
		pointSize = 72;
	}
	[_logViewController clear:self];
	BOOL	isColor = sampleCSPopupButton.indexOfSelectedItem == 1;
	NSInteger	options = formatPopupButton.selectedTag;
	[arduinoDisplayView loadSample:sampleTextField.stringValue pointSize:pointSize
			fontURL:fontPathControl.URL
				options:options
				faceIndex:facePopupButton.indexOfSelectedItem
				supplementalFontURL:suppFontPathControl.URL
				supplementalFaceIndex:suppFacePopupButton.indexOfSelectedItem
				textColor:isColor ? ((NSTextButtonCell*)textColorButton.cell).fillColor : NSColor.whiteColor
				textBGColor:isColor ? ((NSTextButtonCell*)textBGColorButton.cell).fillColor : NSColor.blackColor
				simulateMono:simulateMonoCheckbox.state
				log:_logViewController];
}

- (IBAction)setTextColor:(id)sender
{
	[NSColorPanel setPickerMask:NSColorPanelColorListModeMask];
	[NSColorPanel setPickerMode:NSColorPanelModeColorList];
	NSColorPanel*	colorPanel = [NSColorPanel sharedColorPanel];
	if (colorPanel)
	{
		colorPanel.target = self;
		colorPanel.action = @selector(updateTextColor:);
		colorPanel.color = ((NSTextButtonCell*)textColorButton.cell).fillColor;
		colorPanel.continuous = YES;
		[self.window beginSheet:colorPanel completionHandler:^(NSModalResponse returnCode)
		{
		}];
	//	[colorPanel makeKeyAndOrderFront:self];
	}
}

- (IBAction)updateTextColor:(NSColorPanel*)colorPanel
{
	NSColor* color = colorPanel.color;
	
	if (colorPanel.isVisible)
	{
		NSData* colorData = [NSKeyedArchiver archivedDataWithRootObject:color requiringSecureCoding:NO error:nil];
		[[NSUserDefaults standardUserDefaults] setObject:colorData forKey:kSampleTextColorStrKey];
		((NSTextButtonCell*)textColorButton.cell).fillColor = color;
		textColorButton.needsDisplay = YES;
		[self updateTextBGColorTextColor];
		[self.window endSheet:colorPanel];
	}
}

- (IBAction)setTextBGColor:(id)sender
{
	[NSColorPanel setPickerMask:NSColorPanelColorListModeMask];
	[NSColorPanel setPickerMode:NSColorPanelModeColorList];
	NSColorPanel*	colorPanel = [NSColorPanel sharedColorPanel];
	if (colorPanel)
	{
		colorPanel.target = self;
		colorPanel.action = @selector(updateTextBGColor:);
		colorPanel.color = ((NSTextButtonCell*)textBGColorButton.cell).fillColor;
		colorPanel.continuous = YES;
		[self.window beginSheet:colorPanel completionHandler:^(NSModalResponse returnCode)
		{
		}];
	//	[colorPanel makeKeyAndOrderFront:self];
	}
}

- (void)updateTextBGColorTextColor
{
	NSMutableParagraphStyle *style = [[NSMutableParagraphStyle alloc] init];
    style.alignment = NSTextAlignmentCenter;
    NSDictionary *attrsDictionary = [NSDictionary dictionaryWithObjectsAndKeys:((NSTextButtonCell*)textColorButton.cell).fillColor, NSForegroundColorAttributeName, style, NSParagraphStyleAttributeName, nil];
    NSAttributedString *attrString = [[NSAttributedString alloc]initWithString:@"a" attributes:attrsDictionary];
    [textBGColorButton setAttributedTitle:attrString];
}

- (IBAction)updateTextBGColor:(NSColorPanel*)colorPanel
{
	NSColor* color = colorPanel.color;
	
	if (colorPanel.isVisible)
	{
		NSData* colorData = [NSKeyedArchiver archivedDataWithRootObject:color requiringSecureCoding:NO error:nil];
		[[NSUserDefaults standardUserDefaults] setObject:colorData forKey:kSampleTextBGColorStrKey];
		((NSTextButtonCell*)textBGColorButton.cell).fillColor = color;
		textBGColorButton.needsDisplay = YES;
		[self.window endSheet:colorPanel];
	}
}

/********************************* showHelp ***********************************/
- (IBAction)showHelp:(id)sender
{
	[_logViewController clear:self];
	// These were opening instructions at one point.  I move it here mainly
	// because the instructions need to be reworked.
	NSURL*	openingInstructionsTextURL = [[NSBundle mainBundle] URLForResource:@"OpeningInstructions" withExtension:@"rtf"];
	NSData* rtfData = [NSData dataWithContentsOfURL:openingInstructionsTextURL];
	NSAttributedString* openingInstructions = [[NSAttributedString alloc] initWithRTF:rtfData documentAttributes:nil];
	[_logViewController.logText appendAttributedString:openingInstructions];
	[_logViewController post];
}

@end
