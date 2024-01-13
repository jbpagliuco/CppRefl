#include "gtest/gtest.h"

#include "ReflectedCode/EnumCode.h"
#include "Reflection/EnumInfo.h"

TEST(EnumTests, EnumTypes)
{
	EXPECT_STREQ(cpprefl::GetReflectedType<ReflectedEnum>().mName, "ReflectedEnum");
	EXPECT_EQ(&cpprefl::ReflectedEnum<ReflectedEnum>().mType, &cpprefl::GetReflectedType<ReflectedEnum>());
}

TEST(EnumTests, EnumValues)
{
	const auto& enumInfo = cpprefl::ReflectedEnum<ReflectedEnum>();

	EXPECT_EQ(enumInfo.mValues.size(), 3);

	EXPECT_STREQ(enumInfo.mValues[0].mName, "EnumValue1");
	EXPECT_EQ(enumInfo.mValues[0].mValue, (int)ReflectedEnum::EnumValue1);

	EXPECT_STREQ(enumInfo.mValues[1].mName, "Bar");
	EXPECT_EQ(enumInfo.mValues[1].mValue, (int)ReflectedEnum::Bar);

	EXPECT_STREQ(enumInfo.mValues[2].mName, "Foo");
	EXPECT_EQ(enumInfo.mValues[2].mValue, (int)ReflectedEnum::Foo);
}