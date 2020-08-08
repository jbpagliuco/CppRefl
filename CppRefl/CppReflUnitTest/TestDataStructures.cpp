#include "TestDataStructures.h"

#include "ReflectionBindings.h"

REFL_BIND_VOID_GLOBAL_FUNCTION(TestGlobalVoidFunction);
void TestGlobalVoidFunction()
{

}

REFL_BIND_GLOBAL_FUNCTION(int, TestGlobalIntFunction);
int TestGlobalIntFunction()
{
	return 1;
}

REFL_BIND_VOID_GLOBAL_FUNCTION(TestGlobalVoidFunctionInt, int);
void TestGlobalVoidFunctionInt(int)
{

}

REFL_BIND_GLOBAL_FUNCTION(int, TestGlobalIntFunctionInt, int);
int TestGlobalIntFunctionInt(int)
{
	return 2;
}


REFL_BIND_VOID_FUNCTION(TestClass, VoidFunction);
void TestClass::VoidFunction()
{

}

REFL_BIND_FUNCTION(int, TestClass, IntFunction);
int TestClass::IntFunction()
{
	return 3;
}

REFL_BIND_VOID_FUNCTION(TestClass, VoidFunctionInt, int);
void TestClass::VoidFunctionInt(int)
{

}

REFL_BIND_FUNCTION(float, TestClass, FloatFunctionInt,int);
float TestClass::FloatFunctionInt(int)
{
	return 4;
}