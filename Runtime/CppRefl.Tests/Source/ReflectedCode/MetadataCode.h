#pragma once

#define TEST_METADATA_CODE() 1

#if TEST_METADATA_CODE()

#include "CppReflMarkup.h"

#include "MetadataCode.reflgen.h"


class REFLECTED ClassWithNoMetadata
{
	GENERATED_REFLECTION_CODE()
};

// Class comment.
class
	REFL_META_RUNTIME("TagNoSpaces")
	REFL_META_RUNTIME("Tag Spaces")
	REFL_META_RUNTIME("Attr", "Value")
	REFL_META_RUNTIME("Attr 2", "Value 2")
	REFL_META_RUNTIME("Attr Number", 12)
	REFL_META_RUNTIME("Attr Unquoted String", ClassWithNoMetadata)
	REFL_META_RUNTIME("Attr Multiple", 1, 2, 3)
	REFL_META_COMPILE("Compile Only Tag")
	REFL_META_COMPILE("Compile Only Attr", "Compile Only Value")
	ClassWithMetadata
{
	GENERATED_REFLECTION_CODE()

public:
	/** Field comment. */
	REFL_META_RUNTIME("Field Tag") REFL_META_RUNTIME("Field Attr", "Value")
	REFL_META_COMPILE("Compile Tag") REFL_META_COMPILE("Compile Attr", "Compile Value")
	bool mBool;

	/** Multline
	 * function
	 * comment.
	 */
	REFL_META_RUNTIME("Method Tag") REFL_META_RUNTIME("Method Attr", "Value")
	REFL_META_COMPILE("Compile Tag") REFL_META_COMPILE("Compile Attr", "Compile Value")
	double Func(int param1, short param2);
};

enum class REFLECTED EnumWithMetadata
{
	GENERATED_REFLECTION_CODE()
};

#endif