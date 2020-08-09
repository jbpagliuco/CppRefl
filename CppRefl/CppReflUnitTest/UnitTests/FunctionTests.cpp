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

// Tests member function reflection information.s
TEST_F(FunctionTest, TestMemberFunctionDataTypes)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestClass");
	EXPECT_NE(reflClass, refl::Class::INVALID);
	
	{
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::VOID);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 0);
		EXPECT_TRUE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("IntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::INT32);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 0);
		EXPECT_TRUE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunctionFloat");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::VOID);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 1);
		EXPECT_EQ(reflFunction.mParameterTypes[0].mDataType, refl::DataType::FLOAT);
		EXPECT_TRUE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = reflClass.GetFunction("FloatFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::FLOAT);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 1);
		EXPECT_EQ(reflFunction.mParameterTypes[0].mDataType, refl::DataType::INT32);
		EXPECT_TRUE(reflFunction.mIsMemberFunction);
	}
}

// Tests invoking member functions.
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
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunctionFloat");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		reflFunction.InvokeMember(obj, 1.0f);
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

// Test global function reflection information.
TEST_F(FunctionTest, TestGlobalFunctionDataTypes)
{
	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalVoidFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::VOID);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 0);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::INT32);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 0);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalVoidFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::VOID);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 1);
		EXPECT_EQ(reflFunction.mParameterTypes[0].mDataType, refl::DataType::INT32);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::INT32);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 1);
		EXPECT_EQ(reflFunction.mParameterTypes[0].mDataType, refl::DataType::INT32);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunctionIntPtr");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		EXPECT_EQ(reflFunction.mReturnType, refl::DataType::INT32);
		EXPECT_EQ(reflFunction.mParameterTypes.size(), 1);
		EXPECT_EQ(reflFunction.mParameterTypes[0].mDataType, refl::DataType::INT32);
		EXPECT_TRUE(reflFunction.mParameterTypes[0].mIsPointer);
		EXPECT_FALSE(reflFunction.mIsMemberFunction);
	}
}

// Test invoking global functions.
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

	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunctionIntPtr");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		int p = 1234;
		auto rv = reflFunction.InvokeGlobal<int, int*>(&p);
		EXPECT_TRUE(rv);
		if (rv) {
			EXPECT_EQ(*rv, p);
		}
	}
}

// Test invoking function with nullptr parameter.
TEST_F(FunctionTest, TestFunctionInvocationNullParam)
{
	{
		const refl::Function& reflFunction = mRegistry.GetFunction("TestGlobalIntFunctionIntPtr");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		auto rv = reflFunction.InvokeGlobal<int, int*>(nullptr);
		EXPECT_TRUE(rv);
		if (rv) {
			EXPECT_EQ(*rv, -1);
		}
	}
}

// Ensure that's improperly invoked functions don't get executed.
static int TestBadFunctionInvocation_NumErrors = 0;
TEST_F(FunctionTest, TestBadFunctionInvocation)
{
	TestBadFunctionInvocation_NumErrors = 0;

	TestClass obj;
	obj.mValue = 1234;

	const refl::Class& reflClass = mRegistry.GetClass("TestClass");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	// Count the number of errors we get.
	int expectedErrors = 0;
	auto errorHandler = [](refl::ErrorType, const char*, const char*, int)
	{
		++TestBadFunctionInvocation_NumErrors;
	};
	refl::SetErrorHandler(errorHandler);

	// Bad return value
	{
		const refl::Function& reflFunction = reflClass.GetFunction("VoidFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);

		// Function does not return a value, so badRV should not exist.
		auto badRV = reflFunction.InvokeMember<int>(obj);
		EXPECT_FALSE(badRV);
		++expectedErrors;
	}



	// Too many arguments
	{
		const refl::Function& reflFunction = reflClass.GetFunction("IntFunction");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		auto rv = reflFunction.InvokeMember<int>(obj, 2);
		EXPECT_FALSE(rv);
		++expectedErrors;
	}

	// Too few arguments
	{
		const refl::Function& reflFunction = reflClass.GetFunction("FloatFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		auto rv = reflFunction.InvokeMember<int>(obj);
		EXPECT_FALSE(rv);
		++expectedErrors;
	}

	// No object for a member function
	{
		const refl::Function& reflFunction = reflClass.GetFunction("FloatFunctionInt");
		EXPECT_NE(reflFunction, refl::Function::INVALID);
		auto rv = reflFunction.InvokeMember<int>(nullptr);
		EXPECT_FALSE(rv);
		++expectedErrors;
	}

	EXPECT_EQ(TestBadFunctionInvocation_NumErrors, expectedErrors);
}