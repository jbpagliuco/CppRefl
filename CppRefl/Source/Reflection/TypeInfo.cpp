#include "TypeInfo.h"

#include "Registry.h"

namespace cpprefl
{
	bool IsIntegerType(TypeKind typeKind)
	{
		switch (typeKind)
		{
		case TypeKind::Uint8:
		case TypeKind::Uint16:
		case TypeKind::Uint32:
		case TypeKind::Uint64:
		case TypeKind::Int8:
		case TypeKind::Int16:
		case TypeKind::Int32:
		case TypeKind::Int64:
			return true;

		default:
			return false;
		}
	}

	bool IsRealType(TypeKind typeKind)
	{
		switch (typeKind)
		{
		case TypeKind::Float:
		case TypeKind::Double:
		case TypeKind::LongDouble:
			return true;

		default:
			return false;
		}
	}

	const ClassInfo& TypeInfo::GetClassInfo() const
	{
		return Registry::GetSystemRegistry().GetClass(mName);
	}

	const EnumInfo& TypeInfo::GetEnumInfo() const
	{
		return Registry::GetSystemRegistry().GetEnum(mName);
	}
}
