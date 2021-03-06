#pragma once

#include <map>
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
	bool mBoolVal						REFLECTED;
	int32_t mIntVal						REFLECTED;
	float mFloatVal						REFLECTED;
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

	int32_t mIntWithAttrs				REFLECTED REFL_TAG("tag") REFL_ATTR("attr", "value");

	int32_t* mIntPtr					REFLECTED;

	NestableStruct mNestableStruct		REFLECTED;
	TestEnum mEnum						REFLECTED;
	
	testnamespace::NamespacedStruct mNamespacedStruct		REFLECTED;

	int mFixedSizeArray[13]				REFLECTED;
	char mFixedSizeString[64]			REFLECTED;

	struct {
		bool mBoolInAnonymousStruct		REFLECTED;
	};

	struct REFLECTED NestedStructDefinition {
		int mIntInNamedStruct			REFLECTED;
	};
	NestedStructDefinition mNestedNamedStruct REFLECTED;

	enum class REFLECTED NestedEnumDefinition : int8_t {
		NV1 REFLECTED,
		NV2 REFLECTED
	};
	NestedEnumDefinition mNestedEnum	REFLECTED;

	union REFLECTED NestedUnionDefinition {
		bool mBool;
		int mInt;
	};
	NestedUnionDefinition mUnion		REFLECTED;

	std::string mStdString				REFLECTED;
	std::vector<int> mVectorOfInts		REFLECTED;
	std::map<std::string, int> mMap		REFLECTED;
};

class REFLECTED TestClass
{
public:
	TestStruct mNestedStruct			REFLECTED;
	int mValue;

public:
	void VoidFunction()					REFLECTED;
	int IntFunction()					REFLECTED;

	void VoidFunctionFloat(float)		REFLECTED;
	float FloatFunctionInt(int)			REFLECTED;
};

void TestGlobalVoidFunction()			REFLECTED;
int TestGlobalIntFunction()				REFLECTED;

void TestGlobalVoidFunctionInt(int)		REFLECTED;
int TestGlobalIntFunctionInt(int)		REFLECTED;

int TestGlobalIntFunctionIntPtr(int*)	REFLECTED;