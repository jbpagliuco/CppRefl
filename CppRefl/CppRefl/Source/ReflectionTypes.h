#pragma once

#include <string>

#include "ReflectionMarkup.h"

namespace refl
{
	// All supported data types.
	enum class REFLECTED DataType
	{
		INVALID = 0,

		BOOL = 1,

		UINT8,
		INT8,
		UINT16,
		INT16,
		UINT32,
		INT32,
		UINT64,
		INT64,

		FLOAT,
		DOUBLE,
		LONG_DOUBLE,

		UNION,

		CLASS, // struct, class, record, whatever

		VOID, // for function return values
	};

	// Contains info about a type (e.g. data type, constness, etc.)
	class TypeInfo
	{
	public:
		static TypeInfo INVALID;

	public:
		// Data type.
		DataType mDataType = DataType::INVALID;
		
		// Size of the data type.
		// NB: This is the size as defined by Clang. Sizes from other compilers may differ.
		size_t mSize = -1;

		// Number of elements in this array.
		size_t mArraySize = -1;

		// Underlying class type, if this type is a class.
		std::string mClassType;
		// Underlying enum type, if this type is an enum.
		std::string mEnumType;

		// Is this field const?
		bool mIsConst = false;
		// Is thie field a pointer?
		bool mIsPointer = false;

		// Is this type a dynamic array/string?
		bool IsDynamic = false;

	public:
		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		bool DeepEquals(const TypeInfo& rhs)const;

		// Is this type a fixed sized array?
		bool IsFixedArray()const { return mArraySize != -1; }
		// Is this type a dynamic array?
		bool IsDynamicArray()const { return IsDynamic; }

		// Is this type a fixed sized string?
		bool IsFixedString()const { return IsFixedArray() && mDataType == DataType::INT8; }
		// Is this type a dynamic string?
		bool IsDynamicString()const { return mClassType == "std::string"; }
	};
}