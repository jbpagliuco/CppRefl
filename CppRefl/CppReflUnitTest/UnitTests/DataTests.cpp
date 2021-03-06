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
			EXPECT_FALSE(field.IsFixedArray());
		}
	};

	auto testPrimitive = [&](const std::string& fieldName, refl::DataType type)
	{
		const refl::Field& field = reflClass.GetField(fieldName);
		testCommon(field, type);

		EXPECT_TRUE(field.IsPrimitive());
		EXPECT_FALSE(field.IsClassType());
		EXPECT_FALSE(field.IsEnumType());
		EXPECT_FALSE(field.IsFixedArray());
		EXPECT_FALSE(field.IsConst());
		EXPECT_FALSE(field.IsPointer());
		EXPECT_FALSE(field.IsFixedString());
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
	testPrimitive("mBoolInAnonymousStruct", refl::DataType::BOOL);

	{
		const refl::Field& pointerField = reflClass.GetField("mIntPtr");
		testCommon(pointerField, refl::DataType::INT32);
		EXPECT_FALSE(pointerField.IsEnumType());
		EXPECT_FALSE(pointerField.IsFixedArray());
		EXPECT_FALSE(pointerField.IsConst());
		EXPECT_FALSE(pointerField.IsFixedString());
		EXPECT_TRUE(pointerField.IsPointer());
	}

	{
		const refl::Field& nestableStructField = reflClass.GetField("mNestableStruct");
		testCommon(nestableStructField, refl::DataType::CLASS);
		EXPECT_EQ(nestableStructField.mTypeInfo.mClassType, "NestableStruct");
		EXPECT_FALSE(nestableStructField.IsEnumType());
		EXPECT_FALSE(nestableStructField.IsFixedArray());
		EXPECT_FALSE(nestableStructField.IsConst());
		EXPECT_FALSE(nestableStructField.IsPointer());
		EXPECT_FALSE(nestableStructField.IsFixedString());
	}

	{
		const refl::Field& enumField = reflClass.GetField("mEnum");
		testCommon(enumField, refl::DataType::INT32);
		EXPECT_TRUE(enumField.IsEnumType());
		EXPECT_FALSE(enumField.IsClassType());
		EXPECT_EQ(enumField.mTypeInfo.mEnumType, "TestEnum");
		EXPECT_FALSE(enumField.IsFixedArray());
		EXPECT_FALSE(enumField.IsConst());
		EXPECT_FALSE(enumField.IsPointer());
		EXPECT_FALSE(enumField.IsFixedString());
	}

	{
		const refl::Field& namespacedField = reflClass.GetField("mNamespacedStruct");
		testCommon(namespacedField, refl::DataType::CLASS);
		EXPECT_EQ(namespacedField.mTypeInfo.mClassType, "testnamespace::NamespacedStruct");
		EXPECT_FALSE(namespacedField.IsEnumType());
		EXPECT_FALSE(namespacedField.IsFixedArray());
		EXPECT_FALSE(namespacedField.IsConst());
		EXPECT_FALSE(namespacedField.IsPointer());
		EXPECT_FALSE(namespacedField.IsFixedString());
	}

	{
		const refl::Field& arrayField = reflClass.GetField("mFixedSizeArray");
		testCommon(arrayField, refl::DataType::INT32);
		EXPECT_FALSE(arrayField.IsEnumType());
		EXPECT_FALSE(arrayField.IsConst());
		EXPECT_FALSE(arrayField.IsPointer());

		EXPECT_TRUE(arrayField.IsFixedArray());
		EXPECT_FALSE(arrayField.IsDynamicArray());
		EXPECT_FALSE(arrayField.IsFixedString());
		EXPECT_FALSE(arrayField.IsDynamicString());
	}

	{
		const refl::Field& stringField = reflClass.GetField("mFixedSizeString");
		testCommon(stringField, refl::DataType::INT8);
		EXPECT_FALSE(stringField.IsEnumType());
		EXPECT_FALSE(stringField.IsConst());
		EXPECT_FALSE(stringField.IsPointer());

		EXPECT_TRUE(stringField.IsFixedArray());
		EXPECT_FALSE(stringField.IsDynamicArray());
		EXPECT_TRUE(stringField.IsFixedString());
		EXPECT_FALSE(stringField.IsDynamicString());
	}

	// Nested named struct
	{
		const refl::Field& structField = reflClass.GetField("mNestedNamedStruct");
		testCommon(structField, refl::DataType::CLASS);
		EXPECT_FALSE(structField.IsEnumType());

		const refl::Class& structClass = structField.GetClass();
		const refl::Field& innerField = structClass.GetField("mIntInNamedStruct");
		EXPECT_NE(innerField, refl::Field::INVALID);
		EXPECT_EQ(innerField.mTypeInfo.mDataType, refl::DataType::INT32);
		EXPECT_EQ(innerField.mQualifiedName, std::string("TestStruct::NestedStructDefinition::") + innerField.mName);
	}

	// Nested enum
	{
		const refl::Field& enumField = reflClass.GetField("mNestedEnum");
		testCommon(enumField, refl::DataType::INT8);
		EXPECT_TRUE(enumField.IsEnumType());

		EXPECT_EQ(enumField.GetEnum().mValueTable.size(), 2);
	}

	// Nested union
	{
		const refl::Field& unionField = reflClass.GetField("mUnion");
		testCommon(unionField, refl::DataType::UNION);
	}

	// std::string
	{
		const refl::Field& field = reflClass.GetField("mStdString");
		testCommon(field, refl::DataType::CLASS);
		EXPECT_EQ(field.mTypeInfo.mClassType, "std::string");
		EXPECT_TRUE(field.IsDynamicString());
	}

	// std::vector
	{
		const refl::Field& field = reflClass.GetField("mVectorOfInts");
		testCommon(field, refl::DataType::CLASS);
		EXPECT_EQ(field.mTypeInfo.mClassType, "std::vector");
		EXPECT_TRUE(field.IsDynamicArray());
		EXPECT_TRUE(field.IsTemplated());
		EXPECT_EQ(field.mTypeInfo.mTemplateTypes.size(), 1);
		EXPECT_EQ(field.mTypeInfo.mTemplateTypes[0].mDataType, refl::DataType::INT32);
	}

	// std::map
	{
		const refl::Field& field = reflClass.GetField("mMap");
		testCommon(field, refl::DataType::CLASS);
		EXPECT_EQ(field.mTypeInfo.mClassType, "std::map");
		EXPECT_TRUE(field.IsTemplated());
		EXPECT_EQ(field.mTypeInfo.mTemplateTypes.size(), 2);
		EXPECT_EQ(field.mTypeInfo.mTemplateTypes[0].mDataType, refl::DataType::CLASS);
		EXPECT_TRUE(field.mTypeInfo.mTemplateTypes[0].IsDynamicString());
		EXPECT_EQ(field.mTypeInfo.mTemplateTypes[1].mDataType, refl::DataType::INT32);
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

	TEST_SIZE(mBoolInAnonymousStruct);

	TEST_SIZE(mNestedNamedStruct);
	TEST_SIZE(mNestedEnum);

	TEST_SIZE(mUnion);

	TEST_SIZE(mStdString);
	TEST_SIZE(mVectorOfInts);
	TEST_SIZE(mMap);

	#undef TEST_SIZE

	// The size of a pointer is actually the pointee type
	EXPECT_EQ(reflClass.GetField("mIntPtr").mTypeInfo.mSize, sizeof(int32_t));

	// The size of a fixed size array is the element type
	EXPECT_EQ(reflClass.GetField("mFixedSizeArray").mTypeInfo.mSize, sizeof(int));
	EXPECT_EQ(reflClass.GetField("mFixedSizeArray").mTypeInfo.mArraySize, 13);

	// The size of a fixed size array is the element type
	EXPECT_EQ(reflClass.GetField("mFixedSizeString").mTypeInfo.mSize, sizeof(char));
	EXPECT_EQ(reflClass.GetField("mFixedSizeString").mTypeInfo.mArraySize, 64);

	// Test the size of the whole class
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
	TEST_OFFSET(mFixedSizeString);

	TEST_OFFSET(mBoolInAnonymousStruct);

	TEST_OFFSET(mNestedNamedStruct);
	TEST_OFFSET(mNestedEnum);

	TEST_OFFSET(mUnion);

	TEST_OFFSET(mStdString);
	TEST_OFFSET(mVectorOfInts);
	TEST_OFFSET(mMap);

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

	EXPECT_EQ(reflClass.mFields.size(), 28);
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

TEST_F(DataTest, TestDataAccess)
{
	const refl::Class& reflClass = mRegistry.GetClass("TestStruct");
	EXPECT_NE(reflClass, refl::Class::INVALID);

	TestStruct testData;
	testData.mBool = true;
	testData.mUChar = 'u';
	testData.mChar = 'c';
	testData.mUInt16 = 16;
	testData.mInt16 = -16;
	testData.mUInt32 = 32;
	testData.mInt32 = -32;
	testData.mUInt64 = 64;
	testData.mInt64 = -64;
	testData.mFloat = 3.14f;
	testData.mDouble = 6.28;
	testData.mLongDouble = 628.01;
	testData.mTypedefInt = 7;
	testData.mIntWithAttrs = 8;
	testData.mIntPtr = &testData.mIntWithAttrs;
	testData.mNestableStruct.mBoolVal = true;
	testData.mNestableStruct.mIntVal = 123;
	testData.mNestableStruct.mFloatVal = 1.23f;
	testData.mEnum = TestEnum::VAL2;
	testData.mNamespacedStruct.mBool = true;
	testData.mNamespacedStruct.mInt = 345;
	for (int i = 0; i < sizeof(testData.mFixedSizeArray) / sizeof(testData.mFixedSizeArray[0]); ++i) {
		testData.mFixedSizeArray[i] = i;
	}
	strncpy_s(testData.mFixedSizeString, "cool data smile", sizeof(testData.mFixedSizeString));
	testData.mBoolInAnonymousStruct = true;
	testData.mNestedNamedStruct.mIntInNamedStruct = 34;
	testData.mNestedEnum = TestStruct::NestedEnumDefinition::NV2;
	testData.mUnion.mInt = 1;
	testData.mStdString = "i am an std::string";
	testData.mVectorOfInts = { 1, 3, 5, 7 };
	testData.mMap = { 
		{ "one", 1 },
		{ "two", 2 },
		{ "three", 3 }
	};

	#define TEST_DATA(var) EXPECT_EQ(*(reflClass.GetField(#var).GetDataPtr<decltype(TestStruct::var)>(&testData)), testData.var)

	TEST_DATA(mBool);
	TEST_DATA(mUChar);
	TEST_DATA(mChar);
	TEST_DATA(mUInt16);
	TEST_DATA(mInt16);
	TEST_DATA(mUInt32);
	TEST_DATA(mInt32);
	TEST_DATA(mUInt64);
	TEST_DATA(mInt64);
	EXPECT_FLOAT_EQ(*(reflClass.GetField("mFloat").GetDataPtr<decltype(TestStruct::mFloat)>(&testData)), testData.mFloat);
	EXPECT_DOUBLE_EQ(*(reflClass.GetField("mDouble").GetDataPtr<decltype(TestStruct::mDouble)>(&testData)), testData.mDouble);
	EXPECT_DOUBLE_EQ(*(reflClass.GetField("mLongDouble").GetDataPtr<decltype(TestStruct::mLongDouble)>(&testData)), testData.mLongDouble);
	TEST_DATA(mTypedefInt);
	TEST_DATA(mIntWithAttrs);
	TEST_DATA(mEnum);
	TEST_DATA(mBoolInAnonymousStruct);
	TEST_DATA(mNestedEnum);

	#undef TEST_DATA

	EXPECT_EQ(**reflClass.GetField("mIntPtr").GetDataPtr<int*>(&testData), *testData.mIntPtr);

	// Nestable struct
	{
		const refl::Field& field = reflClass.GetField("mNestableStruct");
		EXPECT_NE(field, refl::Field::INVALID);

		void* nestedClassData = field.GetRawDataPtr(&testData);
		
		const refl::Class& nestedClass = mRegistry.GetClass(field.mTypeInfo.mClassType);
		EXPECT_EQ(*nestedClass.GetField("mBoolVal").GetDataPtr<bool>(nestedClassData), testData.mNestableStruct.mBoolVal);
		EXPECT_EQ(*nestedClass.GetField("mIntVal").GetDataPtr<int>(nestedClassData), testData.mNestableStruct.mIntVal);
		EXPECT_FLOAT_EQ(*nestedClass.GetField("mFloatVal").GetDataPtr<float>(nestedClassData), testData.mNestableStruct.mFloatVal);
	}

	// Namespaced struct
	{
		const refl::Field& field = reflClass.GetField("mNamespacedStruct");
		EXPECT_NE(field, refl::Field::INVALID);

		void* namespacedClassData = field.GetRawDataPtr(&testData);

		const refl::Class& nestedClass = mRegistry.GetClass(field.mTypeInfo.mClassType);
		EXPECT_EQ(*nestedClass.GetField("mBool").GetDataPtr<bool>(namespacedClassData), testData.mNamespacedStruct.mBool);
		EXPECT_EQ(*nestedClass.GetField("mInt").GetDataPtr<int>(namespacedClassData), testData.mNamespacedStruct.mInt);
	}

	// Array
	{
		const refl::Field& field = reflClass.GetField("mFixedSizeArray");
		EXPECT_NE(field, refl::Field::INVALID);

		for (int i = 0; i < sizeof(testData.mFixedSizeArray) / sizeof(testData.mFixedSizeArray[0]); ++i) {
			EXPECT_EQ(*field.GetArrayElement<int>(&testData, i), i);
		}
	}

	// String
	{
		const refl::Field& field = reflClass.GetField("mFixedSizeString");
		EXPECT_NE(field, refl::Field::INVALID);

		char* str = field.GetString(&testData);
		EXPECT_EQ(strcmp(str, testData.mFixedSizeString), 0);
		EXPECT_EQ(strlen(str), strlen(testData.mFixedSizeString));
	}

	// Named nested struct
	{
		const refl::Field& field = reflClass.GetField("mNestedNamedStruct");
		EXPECT_NE(field, refl::Field::INVALID);

		void* nestedClassData = field.GetRawDataPtr(&testData);

		const refl::Class& nestedClass = field.GetClass();
		EXPECT_EQ(*nestedClass.GetField("mIntInNamedStruct").GetDataPtr<int>(nestedClassData), testData.mNestedNamedStruct.mIntInNamedStruct);
	}

	// Union
	{
		const refl::Field& field = reflClass.GetField("mUnion");
		EXPECT_NE(field, refl::Field::INVALID);

		int* unionDataAsInt = field.GetDataPtr<int>(&testData);
		EXPECT_EQ(*unionDataAsInt, testData.mUnion.mInt);

		bool* unionDataAsBool = field.GetDataPtr<bool>(&testData);
		EXPECT_EQ(*unionDataAsBool, testData.mUnion.mBool);
	}

	// std::string
	{
		const refl::Field& field = reflClass.GetField("mStdString");
		EXPECT_NE(field, refl::Field::INVALID);
		
		std::string* string = field.GetDataPtr<std::string>(&testData);
		EXPECT_EQ(*string, testData.mStdString);
	}

	// std::vector
	{
		const refl::Field& field = reflClass.GetField("mVectorOfInts");
		EXPECT_NE(field, refl::Field::INVALID);

		std::vector<int>* vector = field.GetDataPtr<std::vector<int>>(&testData);
		EXPECT_EQ(vector->size(), testData.mVectorOfInts.size());
		EXPECT_TRUE(std::equal(vector->begin(), vector->end(), testData.mVectorOfInts.begin()));
	}

	// std::vector
	{
		const refl::Field& field = reflClass.GetField("mMap");
		EXPECT_NE(field, refl::Field::INVALID);

		std::map<std::string, int>* map = field.GetDataPtr<std::map<std::string, int>>(&testData);
		EXPECT_EQ(map->size(), testData.mMap.size());
		EXPECT_TRUE(std::equal(map->begin(), map->end(), testData.mMap.begin()));
	}
}