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

	auto testCommon = [&](const refl::Field& field, refl::DataType type)
	{
		EXPECT_NE(field, refl::Field::INVALID);
		EXPECT_EQ(field.mTypeInfo.mDataType, type);
		EXPECT_EQ(field.mQualifiedName, std::string("TestStruct::") + field.mName);
		
		if (type == refl::DataType::CLASS) {
			EXPECT_FALSE(field.IsPrimitive());
			EXPECT_TRUE(field.IsClassType());
			EXPECT_FALSE(field.IsFixedSizeArray());
		}
		else {
			EXPECT_FALSE(field.IsString());
		}
	};

	auto testPrimitive = [&](const std::string& fieldName, refl::DataType type)
	{
		const refl::Field& field = reflClass.GetField(fieldName);
		testCommon(field, type);

		EXPECT_TRUE(field.IsPrimitive());
		EXPECT_FALSE(field.IsClassType());
		EXPECT_FALSE(field.IsEnumType());
		EXPECT_FALSE(field.IsFixedSizeArray());
		EXPECT_FALSE(field.IsConst());
		EXPECT_FALSE(field.IsPointer());
	};

	testPrimitive("mBool", refl::DataType::BOOL);

	testPrimitive("mUChar", refl::DataType::UINT8);
	testPrimitive("mChar", refl::DataType::INT8);
	testPrimitive("mInt8", refl::DataType::INT8);

	testPrimitive("mUInt16", refl::DataType::UINT16);
	testPrimitive("mInt16", refl::DataType::INT16);

	testPrimitive("mUInt32", refl::DataType::UINT32);
	testPrimitive("mInt32", refl::DataType::INT32);

	testPrimitive("mUInt64", refl::DataType::UINT64);
	testPrimitive("mInt64", refl::DataType::INT64);

	testPrimitive("mFloat", refl::DataType::FLOAT);
	testPrimitive("mDouble", refl::DataType::DOUBLE);
	testPrimitive("mLongDouble", refl::DataType::LONG_DOUBLE);

	testPrimitive("mTypedefInt", refl::DataType::INT32);

	{
		const refl::Field& pointerField = reflClass.GetField("mIntPtr");
		testCommon(pointerField, refl::DataType::INT32);
		EXPECT_FALSE(pointerField.IsEnumType());
		EXPECT_FALSE(pointerField.IsFixedSizeArray());
		EXPECT_FALSE(pointerField.IsConst());
		EXPECT_TRUE(pointerField.IsPointer());
	}

	{
		const refl::Field& nestableStructField = reflClass.GetField("mNestableStruct");
		testCommon(nestableStructField, refl::DataType::CLASS);
		EXPECT_EQ(nestableStructField.mTypeInfo.mClassType, "NestableStruct");
		EXPECT_FALSE(nestableStructField.IsEnumType());
		EXPECT_FALSE(nestableStructField.IsFixedSizeArray());
		EXPECT_FALSE(nestableStructField.IsConst());
		EXPECT_FALSE(nestableStructField.IsPointer());
	}

	{
		const refl::Field& enumField = reflClass.GetField("mEnum");
		testCommon(enumField, refl::DataType::INT32);
		EXPECT_TRUE(enumField.IsEnumType());
		EXPECT_FALSE(enumField.IsClassType());
		EXPECT_EQ(enumField.mTypeInfo.mEnumType, "TestEnum");
		EXPECT_FALSE(enumField.IsFixedSizeArray());
		EXPECT_FALSE(enumField.IsConst());
		EXPECT_FALSE(enumField.IsPointer());
	}

	{
		const refl::Field& namespacedField = reflClass.GetField("mNamespacedStruct");
		testCommon(namespacedField, refl::DataType::CLASS);
		EXPECT_EQ(namespacedField.mTypeInfo.mClassType, "testnamespace::NamespacedStruct");
		EXPECT_FALSE(namespacedField.IsEnumType());
		EXPECT_FALSE(namespacedField.IsFixedSizeArray());
		EXPECT_FALSE(namespacedField.IsConst());
		EXPECT_FALSE(namespacedField.IsPointer());
	}

	{
		const refl::Field& arrayField = reflClass.GetField("mFixedSizeArray");
		testCommon(arrayField, refl::DataType::INT32);
		EXPECT_FALSE(arrayField.IsEnumType());
		EXPECT_FALSE(arrayField.IsConst());
		EXPECT_FALSE(arrayField.IsPointer());
		EXPECT_TRUE(arrayField.IsFixedSizeArray());
	}
}

TEST_F(DataTest, TestFieldDataSizes)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestStruct");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	#define TEST_SIZE(var) EXPECT_EQ(reflClass.GetField(#var).mTypeInfo.mSize, sizeof(decltype(TestStruct::var)));

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

	TEST_SIZE(mIntWithAttrs);

	TEST_SIZE(mNestableStruct);
	TEST_SIZE(mEnum);
	TEST_SIZE(mNamespacedStruct);

	#undef TEST_SIZE

	// The size of a pointer is actually the pointee type
	EXPECT_EQ(reflClass.GetField("mIntPtr").mTypeInfo.mSize, sizeof(int32_t));

	// The size of a fixed size array is the element type
	EXPECT_EQ(reflClass.GetField("mFixedSizeArray").mTypeInfo.mSize, sizeof(int));

	// The size of a fixed size array is the element type
	EXPECT_EQ(reflClass.GetField("mFixedSizeArray").mTypeInfo.mSize, sizeof(int));
	EXPECT_EQ(reflClass.GetField("mFixedSizeArray").mTypeInfo.mArraySize, 13);

	EXPECT_EQ(reflClass.mSize, sizeof(TestStruct));
}

TEST_F(DataTest, TestFieldDataOffsets)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestStruct");
	EXPECT_NE(reflClass, refl::Class::INVALID);

#define TEST_OFFSET(var) EXPECT_EQ(reflClass.GetField(#var).mOffset, offsetof(TestStruct, var));

	TEST_OFFSET(mBool);

	TEST_OFFSET(mUChar);
	TEST_OFFSET(mChar);
	TEST_OFFSET(mInt8);

	TEST_OFFSET(mUInt16);
	TEST_OFFSET(mInt16);

	TEST_OFFSET(mUInt32);
	TEST_OFFSET(mInt32);

	TEST_OFFSET(mUInt64);
	TEST_OFFSET(mInt64);

	TEST_OFFSET(mFloat);
	TEST_OFFSET(mDouble);
	TEST_OFFSET(mLongDouble);

	TEST_OFFSET(mTypedefInt);

	TEST_OFFSET(mIntWithAttrs);

	TEST_OFFSET(mIntPtr);

	TEST_OFFSET(mNestableStruct);
	TEST_OFFSET(mEnum);
	TEST_OFFSET(mNamespacedStruct);

	TEST_OFFSET(mFixedSizeArray);

#undef TEST_OFFSET
}

TEST_F(DataTest, TestAttrs)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestStruct");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	const refl::Field& field = reflClass.GetField("mIntWithAttrs");
	EXPECT_EQ(field.mAttributes.size(), 2);
	EXPECT_TRUE(field.HasAttribute("tag"));
	EXPECT_TRUE(field.HasAttribute("attr"));
	EXPECT_EQ(field.GetAttribute("attr"), "value");
}

TEST_F(DataTest, TestClassInfo)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestStruct");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	EXPECT_EQ(reflClass.mQualifiedName, "TestStruct");

	EXPECT_EQ(reflClass.mFields.size(), 20);
	EXPECT_EQ(reflClass.mFunctions.size(), 0);
}

TEST_F(DataTest, TestEnumInfo)
{
	const refl::Enum& reflEnum = mRegistry.GetEnum("TestEnum");
	EXPECT_NE(reflEnum, refl::Enum::INVALID);

	EXPECT_EQ(reflEnum.mValueTable.size(), 3);

	EXPECT_EQ(reflEnum.GetValueString((int)TestEnum::VAL1), "VAL1");
	EXPECT_EQ(reflEnum.GetValueString((int)TestEnum::VAL2), "VAL2");
	EXPECT_EQ(reflEnum.GetValueString((int)TestEnum::VAL3), "VAL3");

	EXPECT_EQ(reflEnum.GetValueString((int)TestEnum::VAL1, true), "TestEnum::VAL1");
	EXPECT_EQ(reflEnum.GetValueString((int)TestEnum::VAL2, true), "TestEnum::VAL2");
	EXPECT_EQ(reflEnum.GetValueString((int)TestEnum::VAL3, true), "TestEnum::VAL3");
}