#pragma once

#include <cstdint>
#include <vector>

#include "CppReflMarkup.h"

#include "SerializerCode.reflgen.h"

// An enum to be deserialized.
enum class REFLECTED DeserializeEnum
{
	EnumValue1 = 0,
	Bar = 10,
	Foo
};

// A class with no nested classes.
class REFLECTED DeserializeSimple
{
	GENERATED_REFLECTION_CODE()

public:
	bool boolValue REFLECTED;
	int intValue REFLECTED;
	int negativeIntValue REFLECTED;
	float floatValue REFLECTED;
	float floatFromIntValue REFLECTED;
	DeserializeEnum enumValue REFLECTED;
	char string[64] REFLECTED;
	const char* dynamicString REFLECTED;
	int intArray[4] REFLECTED;
	std::vector<int> dynamicArray REFLECTED;
};

class REFLECTED DeserializeDynamicArrayElement
{
	GENERATED_REFLECTION_CODE()

public:
	bool mBool REFLECTED;
};

// A class with a nested class.
class REFLECTED DeserializeNested
{
	GENERATED_REFLECTION_CODE()

public:
	bool outerBool REFLECTED;
	DeserializeSimple nested REFLECTED;
	int outerInt REFLECTED;
	std::vector<DeserializeDynamicArrayElement> arrayOfClasses REFLECTED;
};

// A class intended to deserialize values out of bounds.
class REFLECTED DeserializeOverflow
{
	GENERATED_REFLECTION_CODE()

public:
	uint8_t mU8 REFLECTED;
	float mFloat REFLECTED;
	int mArray[3] REFLECTED;
	int mPadding = 1;
};

// A class with metadata to be deserialized.
class REFLECTED DeserializeMetadata
{
	GENERATED_REFLECTION_CODE()

public:
	bool enableDamage REFLECTED;
	float damage REFL_ATTR("enable", enableDamage) = 0.0f;

	int mult REFL_ATTR("multiplier", 2);
};


class REFLECTED DeserializationBase
{
	GENERATED_REFLECTION_CODE()

public:
	inline bool GetBaseField()const { return mBaseField; }

protected:
	bool mBaseField REFLECTED;
};

class REFLECTED DeserializationClass1 : public DeserializationBase
{
	GENERATED_REFLECTION_CODE()

public:
	int mInt REFLECTED;
};

class REFLECTED DeserializationClass2 : public DeserializationBase
{
	GENERATED_REFLECTION_CODE()

public:
	const char* mString REFLECTED;
};

class REFLECTED DeserializationDynamic
{
	GENERATED_REFLECTION_CODE()

public:
	std::vector<DeserializationBase*> mInstances REFLECTED;
};