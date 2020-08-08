#pragma once

#include <string>
#include <vector>

#include "ReflectionMarkup.h"

typedef int32_t TypedefInt;

enum class REFLECTED TestEnum : int32_t
{
	VAL1 = 0,
	VAL2 = 1234,
	VAL3 = -1
};

struct REFLECTED NestableStruct
{
	bool mBoolVal;
	int32_t mIntVal;
	float mFloatVal;
};

namespace testnamespace
{
	struct REFLECTED NamespacedStruct
	{
		bool mBool						REFLECTED;
		int32_t mInt					REFLECTED;
	};
}

struct REFLECTED TestStruct
{
	bool mBool							REFLECTED;

	unsigned char mUChar				REFLECTED;
	char mChar							REFLECTED;
	int8_t mInt8						REFLECTED;

	uint16_t mUInt16					REFLECTED;
	int16_t mInt16						REFLECTED;

	uint32_t mUInt32					REFLECTED;
	int32_t mInt32						REFLECTED;

	uint64_t mUInt64					REFLECTED;
	int64_t mInt64						REFLECTED;

	float mFloat						REFLECTED;
	double mDouble						REFLECTED;
	long double mLongDouble				REFLECTED;

	TypedefInt mTypedefInt				REFLECTED;

	std::string mString					REFLECTED;

	std::vector<int32_t> mArrayOfInts	REFLECTED;

	int32_t mIntWithAttrs				REFLECTED REFL_TAG("tag") REFL_ATTR("attr", "value");

	int32_t* mIntPtr					REFLECTED;

	NestableStruct mNestableStruct		REFLECTED;
	TestEnum mEnum						REFLECTED;
	
	testnamespace::NamespacedStruct mNamespacedStruct		REFLECTED;
};

class REFLECTED TestClass
{
public:
	TestStruct mNestedStruct			REFLECTED;

public:
	void VoidFunction()					REFLECTED;
	int IntFunction()					REFLECTED;

	void VoidFunctionInt(int)			REFLECTED;
	int IntFunctionInt(int)				REFLECTED;
};

void TestGlobalVoidFunction()			REFLECTED;
int TestGlobalIntFunction()				REFLECTED;

void TestGlobalVoidFunctionInt(int)		REFLECTED;
int TestGlobalIntFunctionInt(int)		REFLECTED;