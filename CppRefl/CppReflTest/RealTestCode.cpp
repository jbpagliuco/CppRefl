#include "RealTestCode.h"

#include "Reflection.h"

namespace testns
{
	REFL_BIND_FUNCTION(testns::TestStruct, FuncVoidNoParams);
	void TestStruct::FuncVoidNoParams()
	{
		printf("TestStruct::FuncVoidNoParams() called.\n");
	}

	REFL_BIND_FUNCTION_RV(int, testns::TestStruct, FuncIntNoParams);
	int TestStruct::FuncIntNoParams()
	{
		printf("TestStruct::FuncIntNoParams() called.\n");
		return 1;
	}
}