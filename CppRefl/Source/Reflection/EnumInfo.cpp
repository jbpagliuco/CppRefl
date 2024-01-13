#include "EnumInfo.h"

namespace cpprefl
{
	const EnumValueInfo* EnumInfo::GetValue(const char* name)const
	{
		for (const auto& value : mValues)
		{
			if (strcmp(value.mName, name) == 0)
			{
				return &value;
			}
		}
		return nullptr;
	}
}