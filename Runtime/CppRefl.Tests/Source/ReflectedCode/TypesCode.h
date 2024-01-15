#pragma once

#include "CppReflMarkup.h"

#include "TypesCode.reflgen.h"

class REFLECTED PrimitiveTypes
{
	GENERATED_REFLECTION_CODE()

	bool mBool REFLECTED;
	unsigned char mUnsignedChar REFLECTED;
	char mChar REFLECTED;
	unsigned short mUnsignedShort REFLECTED;
	short mShort REFLECTED;
	unsigned int mUnsignedInt REFLECTED;
	int mInt REFLECTED;
	unsigned long mUnsignedLong REFLECTED;
	long mLong REFLECTED;
	unsigned long long mUnsignedLongLong REFLECTED;
	long long mLongLong REFLECTED;

	float mFloat REFLECTED;
	double mDouble REFLECTED;
	long double mLongDouble REFLECTED;

	char mString[64] REFLECTED;
	char* mDynamicString REFLECTED;
	int mIntArray[12] REFLECTED;

	const bool mConstBool REFLECTED = false;
};