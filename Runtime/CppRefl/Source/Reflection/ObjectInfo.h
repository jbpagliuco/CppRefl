#pragma once

#include <string_view>

#include "Span.h"

namespace cpprefl
{
	using TagType = const char*;
	using TagView = Span<TagType>;

	struct Attribute
	{
		constexpr Attribute(const char* value) : mString(value) {}
		constexpr Attribute(int value) : mInt(value) {}
		constexpr Attribute(float value) : mFloat(value) {}

		constexpr operator const char*() const { return mString; }
		constexpr operator int() const { return mInt; }
		constexpr operator float() const { return mFloat; }

		union
		{
			const char* mString;
			int mInt;
			float mFloat;
		};
	};

	using AttributeKeyType = const char*;
	using AttributeValueType = Attribute;
	using AttributeType = std::pair<AttributeKeyType, AttributeValueType>;
	using AttributeView = Span<AttributeType>;

	// Base class for all reflected elements (classes, fields, etc.)
	class ObjectInfo
	{
	public:
		constexpr ObjectInfo() = default;

		constexpr ObjectInfo(TagView&& tags, AttributeView&& attributes) : mTags(tags), mAttributes(attributes)
		{
		}

		// List of reflected tags.
		TagView mTags;

		// List of reflected attributes.
		AttributeView mAttributes;

	public:
		constexpr bool HasTag(const TagType& tag)const { return mTags.Contains(tag, TagEquals); }

		constexpr bool HasAttribute(const AttributeKeyType& key)const { return mAttributes.Contains(key, AttributeKeyEquals); }
		constexpr const AttributeValueType* GetAttribute(const AttributeKeyType& key)const
		{
			const auto value = mAttributes.Find(key, AttributeKeyEquals);
			return value != nullptr ? &value->second : nullptr;
		}

	private:
		static constexpr bool TagEquals(const TagType& tag1, const TagType& tag2) { return std::string_view(tag1) == std::string_view(tag2); }
		static constexpr bool AttributeKeyEquals(const AttributeType& attr1, const AttributeKeyType& attr2) { return std::string_view(attr1.first) == std::string_view(attr2); }
	};
}