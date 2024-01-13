#include "gtest/gtest.h"

#include "ReflectedCode/EnumCode.h"
#include "ReflectedCode/TypesCode.h"

#include "Reflection/ClassInfo.h"
#include "Reflection/EnumInfo.h"
#include "Reflection/FieldInfo.h"
#include "Reflection/TypeInfo.h"

template <typename T>
void TestType(const char* expectedName, const cpprefl::TypeInfo& typeInfo = cpprefl::GetReflectedType<T>())
{
	EXPECT_STREQ(typeInfo.mName, expectedName);
	EXPECT_EQ(typeInfo.mSize, sizeof(T));
}

TEST(StaticTests, PrimitiveTypes)
{
	TestType<bool>("bool");

	TestType<unsigned char>("unsigned char");
	TestType<char>("char");

	TestType<unsigned short>("unsigned short");
	TestType<short>("short");

	TestType<unsigned int>("unsigned int");
	TestType<int>("int");

	TestType<unsigned long>("unsigned long");
	TestType<long>("long");

	TestType<unsigned long long>("unsigned long long");
	TestType<long long>("long long");

	TestType<float>("float");
	TestType<double>("double");
	TestType<long double>("long double");

	TestType<char>("char", PrimitiveTypes::StaticReflectedClass().GetField("mString")->GetType());
	TestType<int>("int", PrimitiveTypes::StaticReflectedClass().GetField("mIntArray")->GetType());

	EXPECT_STREQ(cpprefl::GetReflectedType<void>().mName, "void");
}

TEST(StaticTests, TypeInstance)
{
	auto field = PrimitiveTypes::StaticReflectedClass().GetField("mString");
	EXPECT_EQ(field->mTypeInstance.mIsArray, true);
	EXPECT_EQ(field->mTypeInstance.mIsConst, false);
	EXPECT_EQ(field->mTypeInstance.IsFixedSizeString(), true);
	EXPECT_EQ(field->mTypeInstance.IsDynamicString(), false);
	EXPECT_EQ(field->mTypeInstance.IsString(), true);
	EXPECT_EQ(field->mTypeInstance.mArraySize, 64);

	field = PrimitiveTypes::StaticReflectedClass().GetField("mDynamicString");
	EXPECT_EQ(field->mTypeInstance.mIsArray, false);
	EXPECT_EQ(field->mTypeInstance.mIsConst, false);
	EXPECT_EQ(field->mTypeInstance.IsFixedSizeString(), false);
	EXPECT_EQ(field->mTypeInstance.IsDynamicString(), true);
	EXPECT_EQ(field->mTypeInstance.IsString(), true);

	field = PrimitiveTypes::StaticReflectedClass().GetField("mIntArray");
	EXPECT_EQ(field->mTypeInstance.mIsArray, true);
	EXPECT_EQ(field->mTypeInstance.mIsConst, false);
	EXPECT_EQ(field->mTypeInstance.IsFixedSizeString(), false);
	EXPECT_EQ(field->mTypeInstance.IsDynamicString(), false);
	EXPECT_EQ(field->mTypeInstance.mArraySize, 12);

	field = PrimitiveTypes::StaticReflectedClass().GetField("mConstBool");
	EXPECT_EQ(field->mTypeInstance.mIsArray, false);
	EXPECT_EQ(field->mTypeInstance.mIsConst, true);
	EXPECT_EQ(field->mTypeInstance.IsFixedSizeString(), false);
	EXPECT_EQ(field->mTypeInstance.IsDynamicString(), false);
}

TEST(StaticTests, ClassTypes)
{
	EXPECT_STREQ(cpprefl::GetReflectedType<PrimitiveTypes>().mName, "PrimitiveTypes");
	EXPECT_EQ(&PrimitiveTypes::StaticReflectedType(), &cpprefl::GetReflectedType<PrimitiveTypes>());
	EXPECT_EQ(&PrimitiveTypes::StaticReflectedClass().mType, &cpprefl::GetReflectedType<PrimitiveTypes>());

	PrimitiveTypes p;
	EXPECT_EQ(&PrimitiveTypes::StaticReflectedClass(), &p.GetReflectedClass());
}

TEST(StaticTests, NameTests)
{
	EXPECT_STREQ(cpprefl::GetTypeName<int>(), "int");
	EXPECT_STREQ(cpprefl::GetTypeName<PrimitiveTypes>(), "PrimitiveTypes");
	EXPECT_STREQ(cpprefl::GetTypeName<ReflectedEnum>(), "ReflectedEnum");
}