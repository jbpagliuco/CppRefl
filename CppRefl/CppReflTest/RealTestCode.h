#pragma once

#include <string>
#include <vector>

#include "ReflectionMarkup.h"

namespace testns
{
	typedef int mySpecialInt;

	enum class REFLECTED TestEnum
	{
		VAL1 = 0,
		VAL2 = 1234,
		RVAL = 82
	};

	struct REFLECTED NestableStruct
	{
		bool b REFLECTED;
	};

	struct REFL_TAG("SomeTag") REFL_ATTR("SomeAttr", "SomeValue") TestStruct
	{
		int i								REFL_TAG("INT");
		mySpecialInt typedefInt				REFL_ATTR("TYPEDEF", "TRUE");
		int* ptr							REFLECTED;
		float f								REFLECTED;
		TestEnum e							REFLECTED;
		std::string string					REFLECTED;

		NestableStruct nestedStruct			REFLECTED;

		std::vector<int> vectorOfInts		REFLECTED;

		void FuncVoidNoParams()						REFLECTED;
		int FuncIntNoParams()						REFLECTED;
		void FuncVoidOneParam(int)					REFLECTED;
		int FuncIntOneParam(NestableStruct)			REFLECTED;
		int FuncIntOneParam2(int)					;
	};

	int TestGlobalFunction();
}