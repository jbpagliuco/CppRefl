#pragma once

namespace refl
{
	// All supported data types.
	enum class DataType
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

		CLASS, // struct, class, record, whatever

		VOID, // for function return values
	};

	// Contains info about a type (e.g. data type, constness, etc.)
	class TypeInfo
	{
	public:
		// Data type.
		DataType mDataType;
		
		// Size of the data type.
		// NB: This is the size as defined by Clang. Sizes from other compilers may differ.
		size_t mSize;

		// Underlying class type, if this type is a class.
		std::string mClassType;
		// Underlying enum type, if this type is an enum.
		std::string mEnumType;

		struct {
			// Is this field const?
			bool mIsConst : 1;
			// Is thie field a pointer?
			bool mIsPointer : 1;

			// Is this field a string?
			bool mIsString : 1;
			// Is this field a fixed-sized array?
			bool mIsFixedArray : 1;
		};
	};
}