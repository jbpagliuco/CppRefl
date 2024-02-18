#include "gtest/gtest.h"

#include "EnumCode.h"

#if TEST_ENUM_CODE()

#include "Reflection/EnumInfo.h"

TEST(EnumTests, EnumTypes)
{
	EXPECT_EQ(cpprefl::GetReflectedType<ReflectedEnum>().mName, cpprefl::Name("ReflectedEnum"));
	EXPECT_EQ(cpprefl::GetReflectedEnum<ReflectedEnum>().mType, &cpprefl::GetReflectedType<ReflectedEnum>());
}

TEST(EnumTests, EnumValues)
{
	const auto& enumInfo = cpprefl::GetReflectedEnum<ReflectedEnum>();

	EXPECT_EQ(enumInfo.mValues.size(), 3);

	EXPECT_EQ(enumInfo.mValues[0].mName, cpprefl::Name("EnumValue1"));
	EXPECT_EQ(enumInfo.mValues[0].mValue, (int)ReflectedEnum::EnumValue1);

	EXPECT_EQ(enumInfo.mValues[1].mName, cpprefl::Name("Bar"));
	EXPECT_EQ(enumInfo.mValues[1].mValue, (int)ReflectedEnum::Bar);

	EXPECT_EQ(enumInfo.mValues[2].mName, cpprefl::Name("Foo"));
	EXPECT_EQ(enumInfo.mValues[2].mValue, (int)ReflectedEnum::Foo);
}

TEST(EnumTests, Concepts)
{
#if CPPREFL_CONCEPTS()
	static_assert(cpprefl::ReflectedType<::ReflectedEnum>);
	static_assert(!cpprefl::ReflectedType<::NonReflectedEnum>);

	static_assert(!cpprefl::ReflectedClass<::ReflectedEnum>);
	static_assert(!cpprefl::ReflectedClass<::NonReflectedEnum>);

	static_assert(cpprefl::ReflectedEnum<::ReflectedEnum>);
	static_assert(!cpprefl::ReflectedEnum<::NonReflectedEnum>);
#else
	static_assert(cpprefl::IsReflectedType_v<::ReflectedEnum>);
	static_assert(!cpprefl::IsReflectedType_v<::NonReflectedEnum>);

	static_assert(!cpprefl::IsReflectedClass_v<::ReflectedEnum>);
	static_assert(!cpprefl::IsReflectedClass_v<::NonReflectedEnum>);

	static_assert(cpprefl::IsReflectedEnum_v<::ReflectedEnum>);
	static_assert(!cpprefl::IsReflectedEnum_v<::NonReflectedEnum>);
#endif
}

#endif