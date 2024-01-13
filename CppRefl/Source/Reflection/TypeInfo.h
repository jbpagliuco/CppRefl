#pragma once

#include <type_traits>

namespace cpprefl
{
	class ClassInfo;
	class EnumInfo;

	enum class TypeKind
	{
		Invalid = 0,

		Bool = 1,

		Uint8,
		Int8,
		Uint16,
		Int16,
		Uint32,
		Int32,
		Uint64,
		Int64,

		Float,
		Double,
		LongDouble,

		Enum,
		Union,
		Class, // struct, class, record, whatever

		Void, // void return type
	};

	bool IsIntegerType(TypeKind typeKind);
	bool IsRealType(TypeKind typeKind);

	template <typename T>
	bool IsSameType(TypeKind typeKind);

	// Information about a reflected type.
	class TypeInfo
	{
	public:
		TypeInfo(const char* name, TypeKind kind, size_t size) : mName(name), mKind(kind), mSize(size)
		{
		}

		// Name of this type (fully qualified).
		const char* mName;

		// Type kind.
		TypeKind mKind;

		// Size of this type.
		size_t mSize;

	public:
		// Returns the class info represented by this type.
		const ClassInfo& GetClassInfo()const;

		// Returns the enum info represented by this type.
		const EnumInfo& GetEnumInfo()const;
	};

	template <typename IntType, typename T>
	constexpr bool IsSameIntType()
	{
		return std::is_integral_v<T> && (std::is_unsigned_v<IntType> == std::is_unsigned_v<T>) && (sizeof(IntType) == sizeof(T));
	}

	template <typename T>
	TypeKind GetTypeKind()
	{
		using UnderlyingType = std::remove_pointer_t<std::remove_all_extents_t<T>>;

		if constexpr (std::is_same_v<UnderlyingType, bool>)
		{
			return TypeKind::Bool;
		}
		else if constexpr (IsSameIntType<uint8_t, UnderlyingType>())
		{
			return TypeKind::Uint8;
		}
		else if constexpr (IsSameIntType<int8_t, UnderlyingType>())
		{
			return TypeKind::Int8;
		}
		else if constexpr (IsSameIntType<uint16_t, UnderlyingType>())
		{
			return TypeKind::Uint16;
		}
		else if constexpr (IsSameIntType<int16_t, UnderlyingType>())
		{
			return TypeKind::Int16;
		}
		else if constexpr (IsSameIntType<uint32_t, UnderlyingType>())
		{
			return TypeKind::Uint32;
		}
		else if constexpr (IsSameIntType<int32_t, UnderlyingType>())
		{
			return TypeKind::Int32;
		}
		else if constexpr (IsSameIntType<uint64_t, UnderlyingType>())
		{
			return TypeKind::Uint64;
		}
		else if constexpr (IsSameIntType<int64_t, UnderlyingType>())
		{
			return TypeKind::Int64;
		}
		else if constexpr (std::is_same_v<UnderlyingType, float>)
		{
			return TypeKind::Float;
		}
		else if constexpr (std::is_same_v<UnderlyingType, double>)
		{
			return TypeKind::Double;
		}
		else if constexpr (std::is_same_v<UnderlyingType, long double>)
		{
			return TypeKind::LongDouble;
		}
		else if constexpr (std::is_enum_v<UnderlyingType>)
		{
			return TypeKind::Enum;
		}
		else if constexpr (std::is_class_v<UnderlyingType>)
		{
			return TypeKind::Class;
		}
	}

	template <typename T>
	bool IsSameType(TypeKind typeKind)
	{
		return GetTypeKind<T>() == typeKind;
	}
}
