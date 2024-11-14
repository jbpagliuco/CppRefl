#pragma once

#include "CppReflConfig.h"

#if CPPREFL_WITH_STL()
#include <vector>
#endif

#include "../CppReflStatics.h"

namespace cpprefl
{
	// Functions that allow access to templated dynamic arrays.
	class DynamicArrayFunctions
	{
	public:
		using SetSizeFunction = void(*)(void* arr, ArraySizeType size);
		using GetDataFunction = void*(*)(void* arr);

		DynamicArrayFunctions(const TypeInstanceInfo& elementType, SetSizeFunction setSizeFunction, GetDataFunction getDataFunction) :
			mElementType(elementType),
			mSetSize(setSizeFunction),
			mGetData(getDataFunction)
		{}

		// Element type.
		TypeInstanceInfo mElementType;

		// Sets the size/capacity of the array.
		SetSizeFunction mSetSize;

		// Returns a pointer to the start of the array data.
		GetDataFunction mGetData;
	};

#if CPPREFL_WITH_STL()
	class StdVectorFunctionsFactory
	{
	private:
		template <typename T>
		static void SetSize(void* obj, ArraySizeType size)
		{
			const auto& arr = static_cast<std::vector<T>*>(obj);
			*arr = std::vector<T>(size);
		}

		template <typename T>
		static void* GetData(void* obj)
		{
			const auto& arr = static_cast<std::vector<T>*>(obj);
			return arr->data();
		}

	public:
		template <typename T>
		static DynamicArrayFunctions Create(const Name& elementTypeName)
		{
			return DynamicArrayFunctions(MakeTypeInstance<T>(elementTypeName), SetSize<T>, GetData<T>);
		}
	};
#endif
}
