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

// Tests reflection information about data for a class.
TEST_F(FunctionTest, TestFunctionDataTypes)
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