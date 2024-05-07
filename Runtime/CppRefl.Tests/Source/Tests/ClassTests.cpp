#include "gtest/gtest.h"

#include "ClassCode.h"

#if TEST_CLASS_CODE()

#include "Reflection/Registry.h"

using namespace cpprefl;

TEST(ClassTests, Names)
{
	EXPECT_EQ(cpprefl::GetTypeName<::ReflectedClass>(), Name("ReflectedClass"));
	EXPECT_EQ(cpprefl::GetTypeName<ReflectedStruct>(), Name("ReflectedStruct"));
	EXPECT_EQ(cpprefl::GetTypeName<TestNamespace::ReflectedClass>(), Name("TestNamespace::ReflectedClass"));
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

TEST(ClassTests, GetField)
{
	const auto& classInfo = ReflectedClass::StaticReflectedClass();

	EXPECT_NE(classInfo.GetField("mPublicInt"), nullptr);
	EXPECT_NE(classInfo.GetField("mConstInt"), nullptr);
	EXPECT_EQ(classInfo.GetField("mNotReflectedInt"), nullptr);
	EXPECT_NE(classInfo.GetField("mProtectedFloat"), nullptr);
	EXPECT_NE(classInfo.GetField("mPrivateChar"), nullptr);
	EXPECT_EQ(classInfo.GetField("blargh"), nullptr);
}

TEST(ClassTests, GetFieldValueUnsafe)
{
	const auto& classInfo = ReflectedClass::StaticReflectedClass();

	void* obj = alloca(sizeof(::ReflectedClass));
	classInfo.Construct(obj);

	EXPECT_NE(classInfo.GetFieldValueUnsafe<int>(obj, "mPublicInt"), nullptr);
	EXPECT_EQ(*classInfo.GetFieldValueUnsafe<int>(obj, "mPublicInt"), 1234);
	EXPECT_NE(classInfo.GetFieldValueUnsafe<float>(obj, "mPublicInt"), nullptr);

	EXPECT_EQ(classInfo.GetFieldValueUnsafe<bool>(obj, "blargh"), nullptr);

	EXPECT_NE(classInfo.GetFieldValueUnsafe<ReflectedStruct>(obj, "mStruct1"), nullptr);
	EXPECT_NE(classInfo.GetFieldValueUnsafe<ReflectedStruct2>(obj, "mStruct1"), nullptr);

	EXPECT_NE(classInfo.GetFieldValueUnsafe<ReflectedStruct>(obj, "mStruct2"), nullptr);
	EXPECT_NE(classInfo.GetFieldValueUnsafe<ReflectedStruct2>(obj, "mStruct2"), nullptr);
}

TEST(ClassTests, GetFieldValueSafe)
{
	const auto& classInfo = ReflectedClass::StaticReflectedClass();

	void* obj = alloca(sizeof(::ReflectedClass));
	classInfo.Construct(obj);

	EXPECT_NE(classInfo.GetFieldValueSafe<int>(obj, "mPublicInt"), nullptr);
	EXPECT_EQ(*classInfo.GetFieldValueSafe<int>(obj, "mPublicInt"), 1234);
	EXPECT_EQ(classInfo.GetFieldValueSafe<float>(obj, "mPublicInt"), nullptr);

	EXPECT_EQ(classInfo.GetFieldValueSafe<bool>(obj, "blargh"), nullptr);

	EXPECT_NE(classInfo.GetFieldValueSafe<ReflectedStruct>(obj, "mStruct1"), nullptr);
	EXPECT_EQ(classInfo.GetFieldValueSafe<ReflectedStruct2>(obj, "mStruct1"), nullptr);

	EXPECT_EQ(classInfo.GetFieldValueSafe<ReflectedStruct>(obj, "mStruct2"), nullptr);
	EXPECT_NE(classInfo.GetFieldValueSafe<ReflectedStruct2>(obj, "mStruct2"), nullptr);
}

TEST(ClassTests, Constructors)
{
	void* obj = alloca(sizeof(::ReflectedClass));
	ReflectedClass::StaticReflectedClass().Construct(obj);

	const auto typedObject = (::ReflectedClass*)obj;
	EXPECT_EQ(typedObject->mPublicInt, 1234);

	ReflectedClass::StaticReflectedClass().Destruct(obj);
	EXPECT_EQ(typedObject->mPublicInt, 666);
}

TEST(ClassTests, Concepts)
{
#if CPPREFL_CONCEPTS()
	static_assert(cpprefl::ReflectedType<::ReflectedClass>);
	static_assert(!cpprefl::ReflectedType<::NonReflectedClass>);

	static_assert(cpprefl::ReflectedClass<::ReflectedClass>);
	static_assert(!cpprefl::ReflectedClass<::NonReflectedClass>);

	static_assert(!cpprefl::ReflectedEnum<::ReflectedClass>);
	static_assert(!cpprefl::ReflectedEnum<::NonReflectedClass>);
#else
	static_assert(cpprefl::IsReflectedType_v<::ReflectedClass>);
	static_assert(!cpprefl::IsReflectedType_v<::NonReflectedClass>);

	static_assert(cpprefl::IsReflectedClass_v<::ReflectedClass>);
	static_assert(!cpprefl::IsReflectedClass_v<::NonReflectedClass>);

	static_assert(!cpprefl::IsReflectedEnum_v<::ReflectedClass>);
	static_assert(!cpprefl::IsReflectedEnum_v<::NonReflectedClass>);
#endif
}

#endif