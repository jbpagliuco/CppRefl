#include <string>
#include <vector>

namespace testns
{
	typedef int mySpecialInt;

	enum class TestEnum
	{
		VAL1 = 0,
		VAL2 = 1234,
		RVAL = 82
	};

	struct NestableStruct
	{
		const bool b = true;
	};

	struct TestStruct
	{
		int i;
		mySpecialInt typedefInt;
		int* ptr;
		float f;
		TestEnum e;
		std::string string;

		NestableStruct nestedStruct;
	};
}