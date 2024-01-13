#include "gtest/gtest.h"

#include "Reflection/Registry.h"
#include "FunctionCode.h"

using namespace cpprefl;

static bool ReflectedFunctionCalled = false;
void ReflectedFunction()
{
	ReflectedFunctionCalled = true;
}

static std::tuple<int, float> ReflectedFunctionWithParamsValues;
void ReflectedFunctionWithParams(int param1, float param2)
{
	ReflectedFunctionWithParamsValues = std::make_tuple(param1, param2);
}

void TestNamespace::ReflectedFunction()
{
}


TEST(FunctionTests, Info)
{
	// ReflectedFunction
	{
		const auto& func = Registry::GetSystemRegistry().GetFunction("ReflectedFunction");
		EXPECT_STREQ(func.mName, "ReflectedFunction");
		EXPECT_EQ(&func.mReturnType, &StaticType<void>());
		EXPECT_EQ(func.mArgumentTypes.size(), 0);
	}

	// ReflectedFunctionWithParams
	{
		const auto& func = Registry::GetSystemRegistry().GetFunction("ReflectedFunctionWithParams");
		EXPECT_STREQ(func.mName, "ReflectedFunctionWithParams");
		EXPECT_EQ(&func.mReturnType, &StaticType<void>());
		EXPECT_EQ(func.mArgumentTypes.size(), 2);
		EXPECT_EQ(func.mArgumentTypes[0], &StaticType<int>());
		EXPECT_EQ(func.mArgumentTypes[1], &StaticType<float>());
	}

	// TestNamespace::ReflectedFunction
	{
		const auto& func = Registry::GetSystemRegistry().GetFunction("TestNamespace::ReflectedFunction");
		EXPECT_STREQ(func.mName, "TestNamespace::ReflectedFunction");
		EXPECT_EQ(&func.mReturnType, &StaticType<void>());
		EXPECT_EQ(func.mArgumentTypes.size(), 0);
	}
}

TEST(FunctionTests, Invokers)
{
	{
		ReflectedFunctionCalled = false;
		const auto& func = Registry::GetSystemRegistry().GetFunction("ReflectedFunction");
		func.Invoke<decltype(ReflectedFunction)>();
		EXPECT_TRUE(ReflectedFunctionCalled);
	}

	{
		ReflectedFunctionWithParamsValues = std::make_tuple(0, 0.0f);
		const auto& func = Registry::GetSystemRegistry().GetFunction("ReflectedFunctionWithParams");
		func.Invoke<decltype(ReflectedFunctionWithParams)>(5, 10.0f);
		EXPECT_EQ(std::get<0>(ReflectedFunctionWithParamsValues), 5);
		EXPECT_EQ(std::get<1>(ReflectedFunctionWithParamsValues), 10.0f);
	}

	// TODO: Test incorrect invokers.
}