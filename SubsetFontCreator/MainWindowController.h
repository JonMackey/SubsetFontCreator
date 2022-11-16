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
*	Copyright © 2018 Jon. All rights reserved.
*/


#import <Cocoa/Cocoa.h>
#import "LogViewController.h"
#import "ArduinoDisplayView.h"
#import "SubsetHelperWindowControler.h"

NS_ASSUME_NONNULL_BEGIN


@interface MainWindowController : NSWindowController
{
	IBOutlet NSPathControl*	fontPathControl;
	IBOutlet NSView*		serialView;
	IBOutlet NSPopUpButton*	formatPopupButton;
	IBOutlet NSPopUpButton*	facePopupButton;
	IBOutlet NSTextField*	pointSizeTextField;
	IBOutlet NSTextField*	subsetTextField;
	IBOutlet NSTextField*	sampleTextField;
	IBOutlet NSButton*		offsetWidth16Radio;
	IBOutlet NSButton*		offsetWidth32Radio;
	IBOutlet NSPopUpButton*	sampleDisplayPopupButton;
	IBOutlet ArduinoDisplayView*	arduinoDisplayView;
	IBOutlet NSButton*		textColorButton;
	IBOutlet NSButton*		textBGColorButton;
	IBOutlet NSPopUpButton*	sampleCSPopupButton;
	IBOutlet NSPopUpButton*	displayOrientationPopupButton;
	IBOutlet NSButton*		simulateMonoCheckbox;
	IBOutlet NSButton*		minimizeHeightCheckbox;
	IBOutlet NSPathControl*	suppFontPathControl;
	IBOutlet NSPopUpButton*	suppFacePopupButton;


}
- (IBAction)open:(id)sender;
- (IBAction)save:(id)sender;
- (IBAction)saveas:(id)sender;
- (IBAction)exportSet:(id)sender;
- (IBAction)changeFormat:(id)sender;
- (IBAction)setOffsetWidthRadios:(NSButton*)sender;
- (IBAction)showHelper:(id)sender;
- (IBAction)setDisplay:(NSPopUpButton*)sender;
- (IBAction)setTextColor:(id)sender;
- (IBAction)setTextBGColor:(id)sender;
- (IBAction)set565Color:(id)sender;




@property (nonatomic, strong) LogViewController *logViewController;
@property (nonatomic, strong) NSViewController *formatViewController;
@property (nonatomic, weak) NSSavePanel *savePanel; // valid only while panel is open
@property (nonatomic, strong) NSArray<NSDictionary*>* arduinoDisplays;

- (void)doOpen:(NSURL*)inDocURL;

@end

NS_ASSUME_NONNULL_END
