#pragma once

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

		operator const char*() const { return mString; }
		operator int() const { return mInt; }
		operator float() const { return mFloat; }

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
		ObjectInfo() = default;

		ObjectInfo(TagView&& tags, AttributeView&& attributes) : mTags(tags), mAttributes(attributes)
		{
		}

		// List of reflected tags.
		TagView mTags;

		// List of reflected attributes.
		AttributeView mAttributes;

	public:
		bool HasTag(const TagType& tag)const;

		bool HasAttribute(const AttributeKeyType& key)const;
		const AttributeValueType* GetAttribute(const AttributeKeyType& key)const;
	};
}