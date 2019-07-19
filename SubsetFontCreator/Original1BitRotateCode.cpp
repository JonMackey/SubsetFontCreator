/************************** CreateRotatedBitmap ******************************/
/*
*	This is for displays where each byte of data represents an 8 bit
*	vertical strip.  inBitmap represents one byte as an 8 bit horizontal strip.
*	This routine converts the horizontal strips to vertical, with optional
*	padding bits at the top to account for centering.
*	The rotated data is created in vertical strip order, one complete row of
*	8-bit bytes, then followed by successive rows, as needed.
*/
uint8_t* SubsetFontCreator::CreateRotatedBitmap(
	const uint8_t*	inBitmap,
	int				inRows,
	int				inWidth,	// Bits
	int				inPadding,	// Top padding, zero bits to pre-position glyph on line
	bool			inBottomUp,	// Nokia and OLED SSD1306 is bottom up
	int&			outRows,
	int&			outCols)
{
	uint8_t*	rBitmap = NULL;
	if (inBitmap)
	{
		outCols = inWidth;
		if (inPadding < 0)
		{
			inPadding = 0;
		}
		outRows = (inPadding + inRows + 7)/8;
		size_t mapSize = outRows * outCols;
		rBitmap = new uint8_t[mapSize];
		memset(rBitmap, 0, mapSize);
		int vRowWidth = ((inWidth + 15)/16) * 2;;
		
		const uint8_t*	vBitMapPtr = inBitmap;
		uint8_t	vMask = 0x80;
		int vCol = 0;
		int hRow = inPadding/8;
		int vBitIndex = 0;
		uint8_t*	firstHRowPtr = rBitmap;
		if (hRow)
		{
			firstHRowPtr += (hRow * outCols);
			hRow = 0;
		}
		uint8_t*	hBitmapPtr = firstHRowPtr;
		inPadding = inPadding%8;
		while (true)
		{
			uint8_t	hMask = inBottomUp ? (1 << inPadding) : (0x80 >> inPadding);
			int vRow = 0;
			while (true)
			{
				if (*vBitMapPtr & vMask)
				{
					*hBitmapPtr |= hMask;
				}
				vRow++;
				if (vRow < inRows)
				{
					vBitMapPtr += vRowWidth;
					if (inBottomUp)
					{
						hMask <<= 1;
					} else
					{
						hMask >>= 1;
					}
					if (hMask == 0)
					{
						hMask = inBottomUp ? 1 : 0x80;
						hBitmapPtr += inWidth;
					}
				} else
				{
					break;
				}
			}
			vBitIndex++;
			if (vBitIndex < inWidth)
			{
				vMask >>= 1;
				if (vMask == 0)
				{
					vCol++;
					vMask = 0x80;
				}
				vBitMapPtr = &inBitmap[vCol];
				hRow++;
				hBitmapPtr = &firstHRowPtr[hRow];
			} else
			{
				break;
			}
		}
	} else
	{
		outCols = 0;
		outRows = 0;
	}
	return(rBitmap);
}
