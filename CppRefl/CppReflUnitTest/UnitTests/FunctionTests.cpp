#include "gtest/gtest.h"

#include "ReflectionGen.h"

#include "../TestDataStructures.h"
#include "Common.h"

// Tests
class FunctionTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		CreateRegistry(mRegistry);
	}

	void TearDown() override
	{
	}

protected:
	refl::Registry mRegistry;
};

TEST_F(FunctionTest, TestMemberFunctionDataTypes)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestClass");
	EXPECT_NE(reflClass, refl::Class::INVALID);
	
	{
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::VOID);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("IntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::INT32);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::VOID);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("FloatFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::FLOAT);
	}
}

TEST_F(FunctionTest, TestGlobalFunctionDataTypes)
{
	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalVoidFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::VOID);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::INT32);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalVoidFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::VOID);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::INT32);
	}
}