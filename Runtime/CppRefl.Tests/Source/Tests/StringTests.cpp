#include "gtest/gtest.h"

#include "CppReflHash.h"
#include "Reflection/Registry.h"

TEST(StringTests, Strlen)
{
	EXPECT_EQ(cpprefl::ConstexprStrlen(""), 0);
	EXPECT_EQ(cpprefl::ConstexprStrlen("a"), 1);
	EXPECT_EQ(cpprefl::ConstexprStrlen("My String"), 9);
}

#if CPPREFL_STORE_NAMES()
TEST(StringTests, StringLookup)
{
	// NB: Names do not automatically add the string lookup.
	// Since this is a class name, code is generated to add the lookup.
	const cpprefl::Name name("ReflectedClass");
	EXPECT_STREQ(name.GetString(), "ReflectedClass");
}
#endif