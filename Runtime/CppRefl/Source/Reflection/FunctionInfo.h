#pragma once

#include <assert.h>

#include "ObjectInfo.h"
#include "../CppReflStatics.h"

namespace cpprefl
{
	using FunctionArgTypesView = Span<const TypeInfo*>;

	// Information about a reflected function.
	class FunctionInfo : public ObjectInfo
	{
	public:
		FunctionInfo(
			const Name& name,
			void* functionAddress,
			const TypeInfo& returnType,
			const FunctionArgTypesView& argumentTypes,
			const MetadataTagView& tags,
			const MetadataAttributeView& attributes) : ObjectInfo(tags, attributes), mName(name), mFunctionAddress(functionAddress), mReturnType(returnType), mArgumentTypes(argumentTypes)
		{
		}

		// Enum value name.
		Name mName;

		// Function address.
		void* mFunctionAddress;

		const TypeInfo& mReturnType;

		FunctionArgTypesView mArgumentTypes;



	public:
		template <typename Function, typename ...ArgTypes, typename ReturnType = std::result_of_t<Function&(ArgTypes...)>>
		ReturnType Invoke(ArgTypes... args)const
		{
			using FunctionType = ReturnType(*)(ArgTypes...);

			// Validate return type.
			assert(IsSameType<ReturnType>(mReturnType));

			// Validate number of function arguments.
			assert(sizeof...(ArgTypes) == mArgumentTypes.size());

			// Validate types of function arguments.
			std::array<const TypeInfo*, sizeof...(ArgTypes)> argTypes;
			int i = 0;
			((argTypes[i++] = &GetReflectedType<decltype(args)>()), ...);
			for (int i = 0; i < mArgumentTypes.size(); ++i)
			{
				assert(argTypes[i] == mArgumentTypes[i]);
			}

			if constexpr (std::is_void_v<ReturnType>)
			{
				((FunctionType)mFunctionAddress)(std::forward<ArgTypes>(args)...);
				return;
			}
			else
			{
				return ((FunctionType)mFunctionAddress)(std::forward<ArgTypes>(args)...);
			}
		}
	};
}
