#include "gtest/gtest.h"

#include "ClassCode.h"
#include "Reflection/Registry.h"

using namespace cpprefl;

TEST(ClassTests, Names)
{
	EXPECT_STREQ(cpprefl::GetTypeName<ReflectedClass>(), "ReflectedClass");
	EXPECT_STREQ(cpprefl::GetTypeName<ReflectedStruct>(), "ReflectedStruct");
	EXPECT_STREQ(cpprefl::GetTypeName<TestNamespace::ReflectedClass>(), "TestNamespace::ReflectedClass");
}

TEST(ClassTests, Super)
{
	static_assert(std::is_same_v<ChildClass::Super, BaseClass>);
}

TEST(ClassTests, DerivedClasses)
{
	const auto derivedClasses = Registry::GetSystemRegistry().GetDerivedClasses<BaseClass>();
	EXPECT_EQ(derivedClasses.size(), 2);
	EXPECT_NE(std::find(derivedClasses.begin(), derivedClasses.end(), &ChildClass::StaticReflectedClass()), derivedClasses.end());
	EXPECT_NE(std::find(derivedClasses.begin(), derivedClasses.end(), &ChildClass2::StaticReflectedClass()), derivedClasses.end());

	EXPECT_TRUE(BaseClass::StaticReflectedClass().IsA<BaseClass>());
	EXPECT_FALSE(BaseClass::StaticReflectedClass().IsA<ChildClass>());
	EXPECT_FALSE(BaseClass::StaticReflectedClass().IsA<ChildClass2>());

	EXPECT_TRUE(ChildClass::StaticReflectedClass().IsA<BaseClass>());
	EXPECT_TRUE(ChildClass::StaticReflectedClass().IsA<ChildClass>());
	EXPECT_FALSE(ChildClass::StaticReflectedClass().IsA<ChildClass2>());

	EXPECT_TRUE(ChildClass2::StaticReflectedClass().IsA<BaseClass>());
	EXPECT_TRUE(ChildClass2::StaticReflectedClass().IsA<ChildClass>());
	EXPECT_TRUE(ChildClass2::StaticReflectedClass().IsA<ChildClass2>());
}

TEST(ClassTests, Constructors)
{
	void* obj = alloca(sizeof(ReflectedClass));
	ReflectedClass::StaticReflectedClass().Construct(obj);

	const auto typedObject = (ReflectedClass*)obj;
	EXPECT_EQ(typedObject->mPublicInt, 1234);

	ReflectedClass::StaticReflectedClass().Destruct(obj);
	EXPECT_EQ(typedObject->mPublicInt, 666);
}