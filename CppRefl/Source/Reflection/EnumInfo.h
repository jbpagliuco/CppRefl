#pragma once

#include <optional>
#include <string_view>

#include "ObjectInfo.h"

namespace cpprefl
{
	class TypeInfo;

	// An individual value within an enum.
	class EnumValueInfo : public ObjectInfo
	{
	public:
		constexpr EnumValueInfo(
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
		constexpr EnumInfo(const TypeInfo& type, EnumValueView values, TagView&& tags, AttributeView&& attributes) : ObjectInfo(std::move(tags), std::move(attributes)), mType(type), mValues(values)
		{
		}

		// The type of this class.
		const TypeInfo& mType;

		// All the values in this enum.
		EnumValueView mValues;

	public:
		constexpr const EnumValueInfo* GetValue(const char* name)const { return mValues.Find(name, EnumEquals); }

	private:
		static constexpr bool EnumEquals(const EnumValueInfo& value1, const char* value2) { return std::string_view(value1.mName) == std::string_view(value2); }
	};
}
