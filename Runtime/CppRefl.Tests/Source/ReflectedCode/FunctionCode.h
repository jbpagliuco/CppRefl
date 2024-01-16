#pragma once

#define TEST_FUNCTION_CODE() 1

#if TEST_FUNCTION_CODE()

#include "CppReflMarkup.h"
#include "FunctionCode.reflgen.h"

void REFLECTED ReflectedFunction();
void REFLECTED ReflectedFunctionWithParams(int param1, float param2);

namespace TestNamespace
{
	void REFLECTED ReflectedFunction();
}

#endif