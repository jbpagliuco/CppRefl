#pragma once

#include "CppReflMarkup.h"
#include "GeneratedCode/FunctionCode.reflgen.h"

void REFLECTED ReflectedFunction();
void REFLECTED ReflectedFunctionWithParams(int param1, float param2);

namespace TestNamespace
{
	void REFLECTED ReflectedFunction();
}