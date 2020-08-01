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
		int* ptr;
		float f;
		TestEnum e;
		std::string string;

		NestableStruct nestedStruct;
	};
}