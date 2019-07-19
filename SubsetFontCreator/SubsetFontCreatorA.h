class SubsetCharcodeIterator
{
public:
							SubsetCharcodeIterator(
								std::string&			inSubset)
								: mSubset(inSubset), mNumCharCodes(0), mNumPairs(0),
									mPairIndex(0), mCurrent(0x10000), mEnd(0), mIsValid(NO),
									mCharCodeIndex(0)
									{Initialize();}
	void					Initialize(void);
	BOOL					IsValid(void)
								{return(mIsValid);}
	uint32_t				GetNumCharCodes(void) const
								{return(mNumCharCodes);}
	uint32_t				Next(void);
	uint32_t				Current(void)
								{return(mCurrent);}
	uint32_t				CharCodeIndex(void)
								{return(mCharCodeIndex);}
protected:
	BOOL		mIsValid;
	std::string	mSubset;
	uint32_t	mNumCharCodes;
	uint32_t	mCharCodeIndex;
	uint32_t	mNumPairs;
	uint32_t	mPairIndex;
	uint32_t	mCurrent;
	uint32_t	mEnd;
};

