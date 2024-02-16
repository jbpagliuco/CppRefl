#include "gtest/gtest.h"

#include "MetadataCode.h"

#if TEST_METADATA_CODE()

#include "Reflection/ClassInfo.h"
#include "Reflection/FieldInfo.h"

TEST(MetadataTests, Tags)
{
	EXPECT_EQ(ClassWithNoMetadata::StaticReflectedClass().mTags.size(), 0);
	EXPECT_EQ(ClassWithNoMetadata::StaticReflectedClass().mTags.begin(), ClassWithNoMetadata::StaticReflectedClass().mTags.end());

	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mTags.size(), 2);
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mTags[0], "TagNoSpaces");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mTags[1], "Tag Spaces");

	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mTags.size(), 1);
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mTags[0], "Field Tag");
}

TEST(MetadataTests, Attributes)
{
	EXPECT_EQ(ClassWithNoMetadata::StaticReflectedClass().mAttributes.size(), 0);
	EXPECT_EQ(ClassWithNoMetadata::StaticReflectedClass().mAttributes.begin(), ClassWithNoMetadata::StaticReflectedClass().mAttributes.end());

	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes.size(), 5);
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[0].first, "Attr");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[0].second, "Value");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[1].first, "Attr 2");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[1].second, "Value 2");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[2].first, "Attr Number");
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[2].second.mInt, 12);
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[3].first, "Attr Unquoted String");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[3].second, "ClassWithNoMetadata");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[4].first, "Attr Multiple");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mAttributes[4].second, "1, 2, 3");

	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mAttributes.size(), 1);
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mAttributes[0].first, "Field Attr");
	EXPECT_STREQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mAttributes[0].second, "Value");
}

#endif