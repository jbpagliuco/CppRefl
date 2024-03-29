#pragma once

#include "CppReflHash.h"
#include "Span.h"

namespace cpprefl
{
	// Value associated with an attribute.
	struct MetadataAttributeValue
	{
		explicit constexpr MetadataAttributeValue(const Name& value) : mString(value) {}
		explicit constexpr MetadataAttributeValue(int value) : mInt(value) {}
		explicit constexpr MetadataAttributeValue(float value) : mFloat(value) {}

		constexpr operator Name() const { return mString; }
		constexpr operator int() const { return mInt; }
		constexpr operator float() const { return mFloat; }

		union
		{
			Name mString;
			int mInt;
			float mFloat;
		};
	};

	using MetadataTag = Name;
	using MetadataTagView = Span<MetadataTag>;

	using MetadataAttributeKey = Name;
	using MetadataAttribute = std::pair<MetadataAttributeKey, MetadataAttributeValue>;
	using MetadataAttributeView = Span<MetadataAttribute>;

	// Base class for all reflected elements (classes, fields, etc.)
	class ObjectInfo
	{
	public:
		constexpr ObjectInfo() = default;

		constexpr ObjectInfo(const MetadataTagView& tags, const MetadataAttributeView& attributes) : mTags(tags), mAttributes(attributes)
		{
		}

		// List of reflected tags.
		MetadataTagView mTags;

		// List of reflected attributes.
		MetadataAttributeView mAttributes;

	public:
		constexpr bool HasTag(const MetadataTag& tag)const { return mTags.Contains(tag, TagEquals); }

		constexpr bool HasAttribute(const MetadataAttributeKey& key)const { return mAttributes.Contains(key, AttributeKeyEquals); }
		constexpr const MetadataAttributeValue* GetAttribute(const MetadataAttributeKey& key)const
		{
			const auto value = mAttributes.Find(key, AttributeKeyEquals);
			return value != nullptr ? &value->second : nullptr;
		}

	private:
		static constexpr bool TagEquals(const MetadataTag& tag1, const MetadataTag& tag2) { return tag1 == tag2; }
		static constexpr bool AttributeKeyEquals(const MetadataAttribute& attr1, const MetadataAttributeKey& attr2) { return attr1.first == attr2; }
	};
}