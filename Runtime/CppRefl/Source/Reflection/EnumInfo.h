#pragma once

#include "CppReflHash.h"
#include "CppReflStatics.h"
#include "ObjectInfo.h"

namespace cpprefl
{
	class TypeInfo;

	// An individual value within an enum.
	class EnumValueInfo : public ObjectInfo
	{
	public:
		constexpr EnumValueInfo(
			const Name& name, 
			int value, 
			const MetadataTagView& tags, 
			const MetadataAttributeView& attributes) : ObjectInfo(tags, attributes), mName(name), mValue(value)
		{
		}

		// Enum value name.
		Name mName;

		// Enum value.
		int mValue;
	};

	using EnumValueView = Span<EnumValueInfo>;

	// Information about a reflected class.
	class EnumInfo : public ObjectInfo
	{
	public:
		constexpr EnumInfo(const TypeInfo* type, const EnumValueView& values, const MetadataTagView& tags, const MetadataAttributeView& attributes) : ObjectInfo(tags, attributes), mType(type), mValues(values)
		{
		}

		// The type of this class.
		const TypeInfo* mType;

		// All the values in this enum.
		EnumValueView mValues;

	public:
		constexpr const EnumValueInfo* GetValue(const Name& name)const { return mValues.Find(name, EnumEquals); }

	private:
		static constexpr bool EnumEquals(const EnumValueInfo& value1, const Name& value2) { return value1.mName == value2; }
	};
}
