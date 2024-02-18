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
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mTags[0], cpprefl::Name("TagNoSpaces"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mTags[1], cpprefl::Name("Tag Spaces"));

	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mTags.size(), 1);
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mTags[0], cpprefl::Name("Field Tag"));
}

TEST(MetadataTests, Attributes)
{
	EXPECT_EQ(ClassWithNoMetadata::StaticReflectedClass().mAttributes.size(), 0);
	EXPECT_EQ(ClassWithNoMetadata::StaticReflectedClass().mAttributes.begin(), ClassWithNoMetadata::StaticReflectedClass().mAttributes.end());

	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes.size(), 5);
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[0].first, cpprefl::Name("Attr"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[0].second, cpprefl::Name("Value"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[1].first, cpprefl::Name("Attr 2"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[1].second, cpprefl::Name("Value 2"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[2].first, cpprefl::Name("Attr Number"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[2].second.mInt, 12);
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[3].first, cpprefl::Name("Attr Unquoted String"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[3].second, cpprefl::Name("ClassWithNoMetadata"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[4].first, cpprefl::Name("Attr Multiple"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mAttributes[4].second, cpprefl::Name("1, 2, 3"));

	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mAttributes.size(), 1);
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mAttributes[0].first, cpprefl::Name("Field Attr"));
	EXPECT_EQ(ClassWithMetadata::StaticReflectedClass().mFields[0].mAttributes[0].second, cpprefl::Name("Value"));
}

#endif