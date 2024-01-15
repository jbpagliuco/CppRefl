#include "gtest/gtest.h"

#include "MetadataCode.h"

#if TEST_METADATA_CODE()

#include "Reflection/ClassInfo.h"
#include "Reflection/FieldInfo.h"

TEST(Metadatatests, Tags)
{
	EXPECT_EQ(ClassWithNoAttributes::StaticReflectedClass().mTags.size(), 0);
	EXPECT_EQ(ClassWithNoAttributes::StaticReflectedClass().mTags.begin(), ClassWithNoAttributes::StaticReflectedClass().mTags.end());

	EXPECT_EQ(ClassWithAttributes::StaticReflectedClass().mTags.size(), 2);
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mTags[0], "SomeTag");
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mTags[1], "Another Tag");

	EXPECT_EQ(ClassWithAttributes::StaticReflectedClass().mFields[0].mTags.size(), 1);
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mFields[0].mTags[0], "FieldTag");
}

TEST(Metadatatests, Attributes)
{
	EXPECT_EQ(ClassWithNoAttributes::StaticReflectedClass().mAttributes.size(), 0);
	EXPECT_EQ(ClassWithNoAttributes::StaticReflectedClass().mAttributes.begin(), ClassWithNoAttributes::StaticReflectedClass().mAttributes.end());

	EXPECT_EQ(ClassWithAttributes::StaticReflectedClass().mAttributes.size(), 2);
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mAttributes[0].first, "SomeAttr");
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mAttributes[0].second, "MyValue");
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mAttributes[1].first, "Second Attr");
	EXPECT_EQ(ClassWithAttributes::StaticReflectedClass().mAttributes[1].second.mInt, 12);

	EXPECT_EQ(ClassWithAttributes::StaticReflectedClass().mFields[0].mAttributes.size(), 2);
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mFields[0].mAttributes[0].first, "FieldAttr");
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mFields[0].mAttributes[0].second, "Value");
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mFields[0].mAttributes[1].first, "NoQuotes");
	EXPECT_STREQ(ClassWithAttributes::StaticReflectedClass().mFields[0].mAttributes[1].second, "ClassWithAttributes");
}

#endif