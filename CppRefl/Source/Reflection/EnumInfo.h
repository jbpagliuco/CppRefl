#pragma once

#include <optional>

#include "ObjectInfo.h"

namespace cpprefl
{
	class TypeInfo;

	// An individual value within an enum.
	class EnumValueInfo : public ObjectInfo
	{
	public:
		EnumValueInfo(
			const char* name, 
			int value, 
			TagView&& tags, 
			AttributeView&& attributes) : ObjectInfo(std::move(tags), std::move(attributes)), mName(name), mValue(value)
		{
		}

		// Enum value name.
		const char* mName;

		// Enum value.
		int mValue;
	};

	using EnumValueView = Span<EnumValueInfo>;

	// Information about a reflected class.
	class EnumInfo : public ObjectInfo
	{
	public:
		EnumInfo(const TypeInfo& type, EnumValueView values, TagView&& tags, AttributeView&& attributes) : ObjectInfo(std::move(tags), std::move(attributes)), mType(type), mValues(values)
		{
		}

		// The type of this class.
		const TypeInfo& mType;

		// All the values in this enum.
		EnumValueView mValues;

	public:
		const EnumValueInfo* GetValue(const char* name)const;
	};
}
