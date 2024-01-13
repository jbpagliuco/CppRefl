#include "gtest/gtest.h"

#include "ReflectedCode/ClassCode.h"
#include "Reflection/Registry.h"

using namespace cpprefl;

TEST(ClassTests, Names)
{
	EXPECT_STREQ(ReflectedClass::StaticClassName(), "ReflectedClass");
	EXPECT_STREQ(ReflectedStruct::StaticClassName(), "ReflectedStruct");
	EXPECT_STREQ(TestNamespace::ReflectedClass::StaticClassName(), "TestNamespace::ReflectedClass");
}

TEST(ClassTests, Super)
{
	static_assert(std::is_same_v<ChildClass::Super, BaseClass>);
}

TEST(ClassTests, DerivedClasses)
{
	const auto derivedClasses = Registry::GetSystemRegistry().GetDerivedClasses<BaseClass>();
	EXPECT_EQ(derivedClasses.size(), 2);
	EXPECT_NE(std::find(derivedClasses.begin(), derivedClasses.end(), &ChildClass::StaticClass()), derivedClasses.end());
	EXPECT_NE(std::find(derivedClasses.begin(), derivedClasses.end(), &ChildClass2::StaticClass()), derivedClasses.end());

	EXPECT_TRUE(BaseClass::StaticClass().IsA<BaseClass>());
	EXPECT_FALSE(BaseClass::StaticClass().IsA<ChildClass>());
	EXPECT_FALSE(BaseClass::StaticClass().IsA<ChildClass2>());

	EXPECT_TRUE(ChildClass::StaticClass().IsA<BaseClass>());
	EXPECT_TRUE(ChildClass::StaticClass().IsA<ChildClass>());
	EXPECT_FALSE(ChildClass::StaticClass().IsA<ChildClass2>());

	EXPECT_TRUE(ChildClass2::StaticClass().IsA<BaseClass>());
	EXPECT_TRUE(ChildClass2::StaticClass().IsA<ChildClass>());
	EXPECT_TRUE(ChildClass2::StaticClass().IsA<ChildClass2>());
}

TEST(ClassTests, Constructors)
{
	void* obj = alloca(sizeof(ReflectedClass));
	ReflectedClass::StaticClass().Construct(obj);

	const auto typedObject = (ReflectedClass*)obj;
	EXPECT_EQ(typedObject->mPublicInt, 1234);

	ReflectedClass::StaticClass().Destruct(obj);
	EXPECT_EQ(typedObject->mPublicInt, 666);
}