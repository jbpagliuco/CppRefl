#include "gtest/gtest.h"

#include "TypesCode.h"

#if TEST_TYPES_CODE()

#include "EnumCode.h"
#include "Reflection/ClassInfo.h"
#include "Reflection/EnumInfo.h"
#include "Reflection/FieldInfo.h"
#include "Reflection/TypeInfo.h"

template <typename T>
void TestType(const cpprefl::Name& expectedName, const cpprefl::TypeInfo& typeInfo = cpprefl::GetReflectedType<T>())
{
	EXPECT_EQ(typeInfo.mName, expectedName);
	EXPECT_EQ(typeInfo.mSize, sizeof(T));
}

TEST(StaticTests, PrimitiveTypes)
{
	TestType<bool>(cpprefl::Name("bool"));

	TestType<unsigned char>(cpprefl::Name("unsigned char"));
	TestType<char>(cpprefl::Name("char"));

	TestType<unsigned short>(cpprefl::Name("unsigned short"));
	TestType<short>(cpprefl::Name("short"));

	TestType<unsigned int>(cpprefl::Name("unsigned int"));
	TestType<int>(cpprefl::Name("int"));

	TestType<unsigned long>(cpprefl::Name("unsigned long"));
	TestType<long>(cpprefl::Name("long"));

	TestType<unsigned long long>(cpprefl::Name("unsigned long long"));
	TestType<long long>(cpprefl::Name("long long"));

	TestType<float>(cpprefl::Name("float"));
	TestType<double>(cpprefl::Name("double"));
	TestType<long double>(cpprefl::Name("long double"));

	TestType<char>(cpprefl::Name("char"), PrimitiveTypes::StaticReflectedClass().GetField(cpprefl::Name("mString"))->GetType());
	TestType<int>(cpprefl::Name("int"), PrimitiveTypes::StaticReflectedClass().GetField(cpprefl::Name("mIntArray"))->GetType());

	EXPECT_EQ(cpprefl::GetReflectedType<void>().mName, cpprefl::Name(cpprefl::Name("void")));
}

TEST(StaticTests, TypeInstance)
{
	auto field = PrimitiveTypes::StaticReflectedClass().GetField(cpprefl::Name("mString"));
	EXPECT_EQ(field->mTypeInstance.mIsArray, true);
	EXPECT_EQ(field->mTypeInstance.mIsConst, false);
	EXPECT_EQ(field->mTypeInstance.IsFixedSizeCString(), true);
	EXPECT_EQ(field->mTypeInstance.IsDynamicCString(), false);
	EXPECT_EQ(field->mTypeInstance.IsCString(), true);
	EXPECT_EQ(field->mTypeInstance.mArraySize, 64);

	field = PrimitiveTypes::StaticReflectedClass().GetField(cpprefl::Name("mDynamicString"));
	EXPECT_EQ(field->mTypeInstance.mIsArray, false);
	EXPECT_EQ(field->mTypeInstance.mIsConst, false);
	EXPECT_EQ(field->mTypeInstance.IsFixedSizeCString(), false);
	EXPECT_EQ(field->mTypeInstance.IsDynamicCString(), true);
	EXPECT_EQ(field->mTypeInstance.IsCString(), true);

	field = PrimitiveTypes::StaticReflectedClass().GetField(cpprefl::Name("mIntArray"));
	EXPECT_EQ(field->mTypeInstance.mIsArray, true);
	EXPECT_EQ(field->mTypeInstance.mIsConst, false);
	EXPECT_EQ(field->mTypeInstance.IsFixedSizeCString(), false);
	EXPECT_EQ(field->mTypeInstance.IsDynamicCString(), false);
	EXPECT_EQ(field->mTypeInstance.mArraySize, 12);

	field = PrimitiveTypes::StaticReflectedClass().GetField(cpprefl::Name("mConstBool"));
	EXPECT_EQ(field->mTypeInstance.mIsArray, false);
	EXPECT_EQ(field->mTypeInstance.mIsConst, true);
	EXPECT_EQ(field->mTypeInstance.IsFixedSizeCString(), false);
	EXPECT_EQ(field->mTypeInstance.IsDynamicCString(), false);
}

TEST(StaticTests, ClassTypes)
{
	EXPECT_EQ(cpprefl::GetReflectedType<PrimitiveTypes>().mName, cpprefl::Name("PrimitiveTypes"));
	EXPECT_EQ(&PrimitiveTypes::StaticReflectedType(), &cpprefl::GetReflectedType<PrimitiveTypes>());
	EXPECT_EQ(PrimitiveTypes::StaticReflectedClass().mType, &cpprefl::GetReflectedType<PrimitiveTypes>());

	PrimitiveTypes p;
	EXPECT_EQ(&PrimitiveTypes::StaticReflectedClass(), &p.GetReflectedClass());
}

TEST(StaticTests, NameTests)
{
	EXPECT_EQ(cpprefl::GetTypeName<int>(), cpprefl::Name("int"));
	EXPECT_EQ(cpprefl::GetTypeName<PrimitiveTypes>(), cpprefl::Name("PrimitiveTypes"));
	EXPECT_EQ(cpprefl::GetTypeName<ReflectedEnum>(), cpprefl::Name("ReflectedEnum"));
}

#endif