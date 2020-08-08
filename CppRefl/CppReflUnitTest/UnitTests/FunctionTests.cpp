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
		EXPECT_EQ(reflFunction.mNumParameters, 0);
		EXPECT_TRUE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("IntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::INT32);
		EXPECT_EQ(reflFunction.mNumParameters, 0);
		EXPECT_TRUE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::VOID);
		EXPECT_EQ(reflFunction.mNumParameters, 1);
		EXPECT_TRUE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("FloatFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::FLOAT);
		EXPECT_EQ(reflFunction.mNumParameters, 1);
		EXPECT_TRUE(reflFunction.mIsMemberFunction);
	}
}

TEST_F(FunctionTest, TestMemberFunctionInvocation)
{
	TestClass obj;
	obj.mValue = 1234;

	const refl::Class& reflClass = mRegistry.GetClass("TestClass");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	{
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		reflFunction.InvokeMember(obj);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("IntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		auto rv = reflFunction.InvokeMember<int>(obj);
		EXPECT_TRUE(rv);
		if (rv) {
			EXPECT_EQ(*rv, obj.mValue);
		}
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		reflFunction.InvokeMember(obj, 1);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("FloatFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		auto rv = reflFunction.InvokeMember<float>(obj, 100);
		EXPECT_TRUE(rv);
		if (rv) {
			EXPECT_FLOAT_EQ(*rv, 100.0f);
		}
	}
}

TEST_F(FunctionTest, TestGlobalFunctionDataTypes)
{
	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalVoidFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::VOID);
		EXPECT_EQ(reflFunction.mNumParameters, 0);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::INT32);
		EXPECT_EQ(reflFunction.mNumParameters, 0);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalVoidFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::VOID);
		EXPECT_EQ(reflFunction.mNumParameters, 1);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::Type::INT32);
		EXPECT_EQ(reflFunction.mNumParameters, 1);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}
}

TEST_F(FunctionTest, TestGlobalFunctionInvocation)
{
	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalVoidFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		reflFunction.InvokeGlobal();
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		auto rv = reflFunction.InvokeGlobal<int>();
		EXPECT_TRUE(rv);
		if (rv) {
			EXPECT_EQ(*rv, 1);
		}
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalVoidFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		reflFunction.InvokeGlobal(1);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		auto rv = reflFunction.InvokeGlobal<int, int>(123456);
		EXPECT_TRUE(rv);
		if (rv) {
			EXPECT_EQ(*rv, 2);
		}
	}
}