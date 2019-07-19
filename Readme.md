# SubsetFontCreator

SubsetFontCreator is an Objective-C OS X app that creates a subset of a font for use on displays such as Color TFT, Monochrome OLED and LCDs used on small MCUs such as Arduino.

In many cases you only need a subset of the characters within a font.  For example, if you're writing a thermometer sketch you probably only need the characters needed to display the temperature.

The source fonts are whatever ttf and otf fonts that are present on your Mac.

<i>Disclaimer: It's assumed you have read the End User License Agreement (EULA) of any font you choose to render on your display regarding the permissibility of storing the bitmapped font on the target device. SubsetFontCreator creates bitmapped subfonts, and according to [The Law on Fonts and Typefaces](https://www.crowdspring.com/blog/font-law-licensing/), bitmapped fonts are not copyright protected, but the act of creating the bitmap may be protected.</i>

<b>Portions of this software are copyright © 2018 The [FreeType](https://www.freetype.org) Project.  All rights reserved.</b>


# Usage
Launch the application, select the font, encoding options and define the subset range string.  On launch the application display instructions for creating the subset range string.  If you use a particular font often, you can save the font and it's settings via shift-command-s.  These saved sets show up in the resents menu.

SubsetFontCreator has two export formats, binary and C header.  The binary format is useful if you plan on storing the font on an EEPROM or SD Card.  The C header format is generally used when you intend to store the font in the MCU's flash memory.  In addition, both formats have the option of exporting the glyph data as a separate binary file.  The separate binary option might be used to store the glyph data on an EEPROM with the font header and lookup tables in the MCU's flash memory.  When this option is selected, the associated C header or binary does not contain the glyph data.  This option is found on the export save panel.

All of the other export options are selected in the application's main window.  These options are:
* Font path - any ttf or otf font supported by FreeType
* Font size - the font size in points.  This size is currently limited to 72.
* Glyph data format - 1 bit, 1 bit rotated, and 8 bit for antialiased text.
* Charcode subset string - a UTF-16 string that defines the set of glyphs to export.  Press "Helper…" to display a window that allows you to create the range string. Type or paste text containing the characters you plan to use into its text field. 
* 16 or 32 bit glyph data offsets.  In general 16 should be used.  The XFont class described below currently doesn't handle 32 bit offsets.

In addition to the primary feature of exporting bitmapped subset fonts, there is a sample feature for viewing sample text with the defined options at the same physical size for various common displays.  Additional displays may be added by editing the ArduinoDisplays property list in the application bundle.  This property list defines the screen dimension in pixels as well as the size of the active area in millimeters.  By combining this information with the Mac's own display information it's possible to display an accurately scaled sample.  If you take a ruler and measure the device sample on a 72dpi screen, and measure the same sample on a 144dpi screen, the measurements will be the same size as the target device. 

![Image](MainWAtLaunch.jpg)

An unscaled sample:

![Image](MainWUnscaledSample.jpg)

A scaled sample 8 bit on a TFT high res display:

![Image](MainWScaledSample.jpg)
![Image](Fly_RGB_Sample.jpg)

A scaled sample, 1 bit on a low res display:

![Image](MainWScaledSample2.jpg)

You can change the sample foreground and background colors.  Color can be applied to any display sample regardless of whether the target device supports color.

A scaled sample on a 1 bit OLED display:

![Image](OLED_Time_Sample.jpg)

The same classes used to create the sample are used in the Arduino sketches.  This will give you a reasonable idea of what the sample will look like on your device.  Using the actual classes in the SubsetFontCreator also makes it easier to debug the non-device specific code using the Xcode IDE's debugger.

Note that SubsetFontCreator calls a C++ class of the same name.  All file creation happens in this class using std C++.  This class could be used as the basis for an app on another OS with minor tweaking (it uses POSIX paths.)  FreeType is already cross platform.

# Glyph Drawing
The primary class is XFont.  All of the other classes are used in combination based on the font storage and the display device being targeted.  As noted earlier, subset fonts can be exported in multiple ways depending on where the font data will be stored.  The data storage is abstracted by the use of data streams.  The DataStream base class is pure virtual.  Subclasses allow the data to be read from SRAM, program memory, or on any device for which a subclass of DataStream has been written.  The generic data stream classes are used indirectly by the XFont specific streams XFontR1BitDataStream and XFont16BitDataStream.  When you choose the export format "C++ Header" without the separate data option, SubsetFontCreator has enough information to generate the C++ code needed to implement the font.  You'll see this auto-generated code at the end of the exported header file.  The comments around this code contain basic usage instructions.  In most cases all that needs to be done is to instantiate a display and tie it to the font.

Like the DataStream classes, all of the display classes are subclassed from a common DisplayController class.  Currently there are classes for the TFT ST777xx family, the OLED SSD1306, and Nokia PCD8544 controllers.  I have a limited number of samples of each.  As you may know the ST777xx family covers quite a few variants so some tweaking may be required.

All drawing is done by calling xFont->DrawStr().  DrawStr is passed a UTF-8 string.  You need to call display->MoveTo() prior to calling DrawStr.  DrawStr can optionally simulate a monospace font by passing it the width of the widest glyph in the string being drawn.  This is generally useful for displaying numbers.  XFont has a WidestGlyph function that can be used to calculate the monospace width.  You can also get this number from within SubsetFontCreator by displaying a sample containing the glyphs to be drawn.  SubsetFontCreator also tells you when a subset is monospace.  In some fonts numbers are monospace even though the entire font isn't (e.g. MyriadPro.)

XFont allows you to set the text foreground and background colors as well as highlight foreground and background colors.  When you enable highlighting you're swapping out the text colors.

Note that XFont is not a subclass of Print nor do I plan to make it a subclass.  I suppose a Print wrapper class could be written at some point.

# Arduino
Under the Arduino folder in the repository are the classes described above and a few simple examples.  Contact me if you have questions about implentation.

# Glyph Data Storage
1 bit data is stored by removing all bit padding and shifting the bits into a contiguous string of bits.  FreeType generates bitmap data that is the exact size of the glyph with padding (e.g for a 6 bit wide glyph there will be 2 bits of padding per row.)  The XFont storage requirements for any glyph are its row and column dimensions rounded up to the nearest byte.  For example, if the glyph dimensions are 6 rows x 6 columns, the number of bytes needed to store it would be abs(((6 * 6) + 7)/8) or 5 bytes.

8 bit data is stored using simple run length encoding.  For each run the data starts with a signed length byte.  If the length byte is positive the next byte should be repeated length times.  If the length byte is negative, -length bytes of unique data should be copied.  This optimizes the case where there are runs of unique values.

XFontR1BitDataStream and XFont16BitDataStream are used to unpack the glyph data.  XFontR1BitDataStream works on rotated 1 bit data as used on monochrome displays such as the OLED SSD1306 and Nokia PCD8544 controllers.  XFont16BitDataStream unpacks both 1 bit unrotated and 8 bit data for use with RGB displays.  For 8 bit data the XFont16BitDataStream handles blending the foreground and background colors to implement antialiasing.  Obviously antialiasing is only used when the resolution of the display is high enough, otherwise 1 bit makes more sense.

If you plan on supporting many fonts in your project, more than the available flash program space available, the glyph data can be stored anywhere.  As a test I wrote a data stream for the AT24Cxxx family of eeproms.  The drawing speed is slightly slower, but it works fine otherwise.

To get font data or any other data onto various storage devices, I wrote an application to do the conversion to Intel HEX format and serially transfer the data to the target device.  See SerialHexLoader for more details.

# Building
The app can be built as a sandboxed app or a regular app.  Only the sandboxed app has been tested.  A built sandboxed app is included in the repository.  A built archive of the FreeType library is included in the repository.  If for some reason you need to build the archive, the FreeType project without the sources is included in the repository.    You can get the FreeType sources from [FreeType](https://download.savannah.gnu.org/releases/freetype/) org, version 2.9.1.  Before building the FreeType archive you should comment out the two zip related defines: FT_CONFIG_OPTION_USE_LZW and FT_CONFIG_OPTION_USE_ZLIB in ftoption.h otherwise you may get missing symbol errors when you try to build the release version of SubsetFontCreator.  The built libfreetype archive needs to be copied to the Library folder within the root SubsetFontCreator folder.

