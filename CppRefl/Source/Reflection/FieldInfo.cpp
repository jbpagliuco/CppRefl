#include "FieldInfo.h"

namespace cpprefl
{
	void* FieldInfo::GetMemoryInClass(void* classObject) const
	{
		return (std::byte*)classObject + mOffset;
	}

	void* FieldInfo::GetClassObject(void* fieldObject) const
	{
		return (std::byte*)fieldObject - mOffset;
	}
}
