#include "gtest/gtest.h"

#include "ReflectionGen.h"

#include "../TestDataStructures.h"
#include "Common.h"

// Tests
class DataTest : public ::testing::Test
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
TEST_F(DataTest, TestFieldDataTypes)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestStruct");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	auto testCommon = [&](const refl::Field& field, refl::Type type)
	{
		EXPECT_NE(field, refl::Field::INVALID);
		EXPECT_EQ(field.mType, type);
		EXPECT_EQ(field.mQualifiedName, std::string("TestStruct::") + field.mName);
		
		if (type == refl::Type::CLASS) {
			EXPECT_FALSE(field.IsPrimitive());
			EXPECT_TRUE(field.IsClassType());
			EXPECT_FALSE(field.IsArray());
		}
		else {
			EXPECT_FALSE(field.IsString());
		}
	};

	auto testPrimitive = [&](const std::string& fieldName, refl::Type type)
	{
		const refl::Field& field = reflClass.GetField(fieldName);
		testCommon(field, type);

		EXPECT_TRUE(field.IsPrimitive());
		EXPECT_FALSE(field.IsClassType());
		EXPECT_FALSE(field.IsEnumType());
		EXPECT_FALSE(field.IsArray());
		EXPECT_FALSE(field.IsConst());
		EXPECT_FALSE(field.IsPointer());
	};

	testPrimitive("mBool", refl::Type::BOOL);

	testPrimitive("mUChar", refl::Type::UINT8);
	testPrimitive("mChar", refl::Type::INT8);
	testPrimitive("mInt8", refl::Type::INT8);

	testPrimitive("mUInt16", refl::Type::UINT16);
	testPrimitive("mInt16", refl::Type::INT16);

	testPrimitive("mUInt32", refl::Type::UINT32);
	testPrimitive("mInt32", refl::Type::INT32);

	testPrimitive("mUInt64", refl::Type::UINT64);
	testPrimitive("mInt64", refl::Type::INT64);

	testPrimitive("mFloat", refl::Type::FLOAT);
	testPrimitive("mDouble", refl::Type::DOUBLE);
	testPrimitive("mLongDouble", refl::Type::LONG_DOUBLE);

	testPrimitive("mTypedefInt", refl::Type::INT32);

	{
		const refl::Field& stringField = reflClass.GetField("mString");
		testCommon(stringField, refl::Type::CLASS);
		EXPECT_EQ(stringField.mClassType, "std::string");
		EXPECT_FALSE(stringField.IsEnumType());
		EXPECT_TRUE(stringField.IsString());
		EXPECT_FALSE(stringField.IsConst());
		EXPECT_FALSE(stringField.IsPointer());
	}

	{
		const refl::Field& vectorOfIntsField = reflClass.GetField("mArrayOfInts");
		testCommon(vectorOfIntsField, refl::Type::INT32); // type here is the vector templated type
		EXPECT_TRUE(vectorOfIntsField.IsArray());			// but it is still an array
		EXPECT_EQ(vectorOfIntsField.mClassType, "std::vector");
		EXPECT_FALSE(vectorOfIntsField.IsEnumType());
		EXPECT_FALSE(vectorOfIntsField.IsConst());
		EXPECT_FALSE(vectorOfIntsField.IsPointer());
	}

	{
		const refl::Field& pointerField = reflClass.GetField("mIntPtr");
		testCommon(pointerField, refl::Type::INT32);
		EXPECT_FALSE(pointerField.IsEnumType());
		EXPECT_FALSE(pointerField.IsArray());
		EXPECT_FALSE(pointerField.IsConst());
		EXPECT_TRUE(pointerField.IsPointer());
	}

	{
		const refl::Field& nestableStructField = reflClass.GetField("mNestableStruct");
		testCommon(nestableStructField, refl::Type::CLASS);
		EXPECT_EQ(nestableStructField.mClassType, "NestableStruct");
		EXPECT_FALSE(nestableStructField.IsEnumType());
		EXPECT_FALSE(nestableStructField.IsArray());
		EXPECT_FALSE(nestableStructField.IsConst());
		EXPECT_FALSE(nestableStructField.IsPointer());
	}

	{
		const refl::Field& enumField = reflClass.GetField("mEnum");
		testCommon(enumField, refl::Type::INT32);
		EXPECT_TRUE(enumField.IsEnumType());
		EXPECT_FALSE(enumField.IsClassType());
		EXPECT_EQ(enumField.mEnumType, "TestEnum");
		EXPECT_FALSE(enumField.IsArray());
		EXPECT_FALSE(enumField.IsConst());
		EXPECT_FALSE(enumField.IsPointer());
	}

	{
		const refl::Field& namespacedField = reflClass.GetField("mNamespacedStruct");
		testCommon(namespacedField, refl::Type::CLASS);
		EXPECT_EQ(namespacedField.mClassType, "testnamespace::NamespacedStruct");
		EXPECT_FALSE(namespacedField.IsEnumType());
		EXPECT_FALSE(namespacedField.IsArray());
		EXPECT_FALSE(namespacedField.IsConst());
		EXPECT_FALSE(namespacedField.IsPointer());
	}
}

TEST_F(DataTest, TestFieldDataSizes)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestStruct");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	#define TEST_SIZE(var) EXPECT_EQ(reflClass.GetField(#var).mSize, sizeof(decltype(TestStruct::var)));

	TEST_SIZE(mBool);

	TEST_SIZE(mUChar);
	TEST_SIZE(mChar);
	TEST_SIZE(mInt8);

	TEST_SIZE(mUInt16);
	TEST_SIZE(mInt16);

	TEST_SIZE(mUInt32);
	TEST_SIZE(mInt32);

	TEST_SIZE(mUInt64);
	TEST_SIZE(mInt64);

	TEST_SIZE(mFloat);
	TEST_SIZE(mDouble);
	TEST_SIZE(mLongDouble);

	TEST_SIZE(mTypedefInt);

	TEST_SIZE(mString);

	TEST_SIZE(mIntWithAttrs);

	TEST_SIZE(mNestableStruct);
	TEST_SIZE(mEnum);
	TEST_SIZE(mNamespacedStruct);

	#undef TEST_SIZE

	// The size of a pointer is actually the pointee type
	EXPECT_EQ(reflClass.GetField("mIntPtr").mSize, sizeof(int32_t));

	// The size of an array is actually the element type
	EXPECT_EQ(reflClass.GetField("mArrayOfInts").mSize, sizeof(int32_t));
}

TEST_F(DataTest, TestAttrs)
{

}

TEST_F(DataTest, TestClassInfo)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestStruct");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	EXPECT_EQ(reflClass.mFields.size(), 21);
	EXPECT_EQ(reflClass.mFunctions.size(), 0);
}