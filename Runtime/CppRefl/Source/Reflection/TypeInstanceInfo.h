#pragma once

#include <cstdint>
#include <type_traits>

#include "TypeInfo.h"
#include "../CppReflStatics.h"

namespace cpprefl
{
	class TypeInfo;

	using ArraySizeType = uint32_t;

	// Information about a reflected type.
	class TypeInstanceInfo
	{
	public:
		constexpr TypeInstanceInfo(const TypeInfo& type, ArraySizeType arraySize, bool isConst, bool isArray, bool isPointer) :
			mType(type),
			mArraySize(arraySize),
			mIsConst(isConst),
			mIsArray(isArray),
			mIsPointer(isPointer)
		{
		}

		const TypeInfo& mType;

		// Fixed array size (only relevant if isArray is true).
		ArraySizeType mArraySize;

		// Modifiers
		struct
		{
			bool mIsConst : 1;
			bool mIsArray : 1;
			bool mIsPointer : 1;
		};

	public:
		bool IsFixedSizeString()const { return mIsArray && mType.mKind == TypeKind::Int8; }
		bool IsDynamicString()const { return mIsPointer && mType.mKind == TypeKind::Int8; }
		bool IsString()const { return IsFixedSizeString() || IsDynamicString(); }
	};

	template <typename T>
	constexpr ArraySizeType ArraySize_v = 0;

	template <typename T, size_t N>
	constexpr ArraySizeType ArraySize_v<T[N]> = N;

	template <typename T, size_t N>
	constexpr ArraySizeType ArraySize_v<std::array<T, N>> = N;

	template <typename T>
	constexpr TypeInstanceInfo MakeTypeInstance(const TypeInfo& type)
	{
		if constexpr (std::is_array_v<T>)
		{
			return TypeInstanceInfo(type,
				ArraySize_v<T>,
				std::is_const_v<T>,
				std::is_array_v<T>,
				false);
		}
		else
		{
			return TypeInstanceInfo(type,
				0,
				std::is_const_v<T>,
				std::is_array_v<T>,
				std::is_pointer_v<T>);
		}
	}

	template <typename T>
	constexpr TypeInstanceInfo MakeTypeInstance(const Name& typeName)
	{
		return MakeTypeInstance<T>(CppReflPrivate::MaybeCreateReflectedType<T>(typeName));
	}
}
