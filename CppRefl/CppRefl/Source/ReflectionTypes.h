#pragma once

namespace refl
{
	// All support data types.
	enum class Type
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
}