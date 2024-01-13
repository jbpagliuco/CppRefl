#pragma once

#include "CppReflMarkup.h"

#include "GeneratedCode/MetadataCode.reflgen.h"


class REFLECTED ClassWithNoAttributes
{
	GENERATED_REFLECTION_CODE()
};

class REFL_TAG("SomeTag") REFL_TAG("Another Tag") REFL_ATTR("SomeAttr", "MyValue") REFL_ATTR("Second Attr", 12) ClassWithAttributes
{
	GENERATED_REFLECTION_CODE()

public:
	REFL_TAG("FieldTag") REFL_ATTR("FieldAttr", "Value") REFL_ATTR("NoQuotes", ClassWithAttributes)
	bool mBool;

	REFL_TAG("FunctionTag") REFL_ATTR("FunctionAttr", "Function")
	double Func(int param1, short param2);
};
