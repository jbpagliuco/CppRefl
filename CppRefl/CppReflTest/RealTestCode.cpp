#include "RealTestCode.h"

#include "Reflection.h"

namespace testns
{
	REFL_BIND_VOID_FUNCTION(testns::TestStruct, FuncVoidNoParams);
	void TestStruct::FuncVoidNoParams()
	{
		printf("TestStruct::FuncVoidNoParams() called.\n");
	}

	REFL_BIND_FUNCTION(int, testns::TestStruct, FuncIntNoParams);
	int TestStruct::FuncIntNoParams()
	{
		printf("TestStruct::FuncIntNoParams() called.\n");
		return 123456;
	}

	REFL_BIND_VOID_FUNCTION(testns::TestStruct, FuncVoidOneParam, int);
	void TestStruct::FuncVoidOneParam(int p)
	{
		printf("TestStruct::FuncIntNoParams(%d) called.\n", p);
	}

	REFL_BIND_FUNCTION(int, testns::TestStruct, FuncIntOneParam, NestableStruct);
	int TestStruct::FuncIntOneParam(NestableStruct p)
	{
		printf("TestStruct::FuncIntNoParams({%s}) called.\n", p.b ? "true" : "false");
		return 1;
	}

	int TestStruct::FuncIntOneParam2(int p)
	{
		return 1;
	}


	int TestGlobalFunction()
	{
		printf("TestGlobalFunction() called.\n");
		return 69;
	}
}