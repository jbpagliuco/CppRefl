#include "gtest/gtest.h"

#include "ReflectedCode/MetadataCode.h"
#include "Reflection/ClassInfo.h"
#include "Reflection/FieldInfo.h"

TEST(Metadatatests, Tags)
{
	EXPECT_EQ(ClassWithNoAttributes::StaticClass().mTags.size(), 0);
	EXPECT_EQ(ClassWithNoAttributes::StaticClass().mTags.begin(), ClassWithNoAttributes::StaticClass().mTags.end());

	EXPECT_EQ(ClassWithAttributes::StaticClass().mTags.size(), 2);
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mTags[0], "SomeTag");
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mTags[1], "Another Tag");

	EXPECT_EQ(ClassWithAttributes::StaticClass().mFields[0].mTags.size(), 1);
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mFields[0].mTags[0], "FieldTag");
}

TEST(Metadatatests, Attributes)
{
	EXPECT_EQ(ClassWithNoAttributes::StaticClass().mAttributes.size(), 0);
	EXPECT_EQ(ClassWithNoAttributes::StaticClass().mAttributes.begin(), ClassWithNoAttributes::StaticClass().mAttributes.end());

	EXPECT_EQ(ClassWithAttributes::StaticClass().mAttributes.size(), 2);
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mAttributes[0].first, "SomeAttr");
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mAttributes[0].second, "MyValue");
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mAttributes[1].first, "Second Attr");
	EXPECT_EQ(ClassWithAttributes::StaticClass().mAttributes[1].second.mInt, 12);

	EXPECT_EQ(ClassWithAttributes::StaticClass().mFields[0].mAttributes.size(), 2);
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mFields[0].mAttributes[0].first, "FieldAttr");
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mFields[0].mAttributes[0].second, "Value");
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mFields[0].mAttributes[1].first, "NoQuotes");
	EXPECT_STREQ(ClassWithAttributes::StaticClass().mFields[0].mAttributes[1].second, "ClassWithAttributes");
}