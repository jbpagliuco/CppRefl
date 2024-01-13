#include "ObjectInfo.h"

namespace cpprefl
{
	bool ObjectInfo::HasTag(const TagType& tag) const
	{
		for (const auto& it : mTags)
		{
			if (strcmp(it, tag) == 0)
			{
				return true;
			}
		}

		return false;
	}

	bool ObjectInfo::HasAttribute(const AttributeKeyType& key) const
	{
		for (const auto& it : mAttributes)
		{
			if (strcmp(it.first, key) == 0)
			{
				return true;
			}
		}

		return false;
	}

	const AttributeValueType* ObjectInfo::GetAttribute(const AttributeKeyType& key) const
	{
		for (const auto& it : mAttributes)
		{
			if (strcmp(it.first, key) == 0)
			{
				return &it.second;
			}
		}

		return nullptr;
	}
}
