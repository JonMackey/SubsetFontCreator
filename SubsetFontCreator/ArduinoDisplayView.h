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
*	ArduinoDisplayView
*	
*	Created by Jon Mackey on 4/29/19.
*	Copyright © 2019 Jon. All rights reserved.
*/


#import <Cocoa/Cocoa.h>
#import "LogViewController.h"

NS_ASSUME_NONNULL_BEGIN

@interface ArduinoDisplayView : NSView
/*{
	Glyph*	glyph;
}
- (void)setGlyph:(Glyph*)inGlyph;*/

@property (nonatomic) CGFloat displayPPMM;
@property (nonatomic) NSSize deviceSize;
@property (nonatomic) NSSize screenSize;
@property (nonatomic) BOOL bitmapIsRotated;

@property (nonatomic, strong) NSBitmapImageRep* bitmapImageRep;

- (void)setDisplay:(NSDictionary*)inDisplayDict;
- (CGFloat)ppmm;	// monitor's native pixels per mm
- (void)loadSample:(NSString*)inSampleText
			pointSize:(NSUInteger)inPointSize
			fontURL:(NSURL*)inFontURL
			isRotated:(BOOL)inIsRotated
			is1BitPerPixel:(BOOL)in1BitPerPixel
			faceIndex:(NSInteger)inFaceIndex
			textColor:(NSColor*)inTextColor
			textBGColor:(NSColor*)inTextBGColor
			simulateMono:(BOOL)inSimulateMono
			log:(LogViewController*__nullable)inLog;
+ (uint16_t)to565Color:(NSColor*)inColor;
@end

NS_ASSUME_NONNULL_END
