#pragma once

#include "ErrorHandling.h"
#include "ReflectionTypes.h"

namespace refl
{
	template<typename T, T> struct FunctionInvoker;

	// Member function invoker.
	template<typename ClassType, typename ReturnType, typename ...ParamTypes, ReturnType (ClassType::*FunctionPtr)(ParamTypes...)>
	struct FunctionInvoker<ReturnType (ClassType::*)(ParamTypes...), FunctionPtr>
	{
		ReturnType Invoke(ClassType* obj, ParamTypes... params)const
		{
			return (obj->*FunctionPtr)(std::forward<ParamTypes>(params)...);
		}
	};

	// Global function invoker.
	/*template <typename ReturnType, typename ...ParamTypes, ReturnType (*FunctionPtr)(ParamTypes...)>
	struct FunctionInvoker<ReturnType(*)(ParamTypes...), FunctionPtr>
	{
		ReturnType operator()(ParamTypes&&... params)
		{
			return (*FunctionPtr)(std::forward<ParamTypes>(params)...);
		}
	};*/


	#define REFL_INTERNAL_INVOKE(...) static_cast<FunctionInvokerType*>(functionInvoker)->Invoke((ClassType*)obj, __VA_ARGS__)

	// No parameters.
	template <typename FunctionInvokerType, typename ClassType>
	void VoidFunctionInvokerWrapper(void* functionInvoker, void* obj, void* rv, void* dummyParam)
	{
		REFL_INTERNAL_INVOKE();
	}

	template <typename FunctionInvokerType, typename ReturnType, typename ClassType>
	void FunctionInvokerWrapper(void* functionInvoker, void* obj, void* rv, void* dummyParam)
	{
		if (rv != nullptr) {
			*((ReturnType*)rv) = REFL_INTERNAL_INVOKE();
		}
		else {
			REFL_INTERNAL_INVOKE();
		}
	}
	
	// One parameter.
	template <typename FunctionInvokerType, typename ClassType, typename ParamType>
	void VoidFunctionInvokerWrapper(void* functionInvoker, void* obj, void* rv, void* param)
	{
		REFL_INTERNAL_INVOKE(*(ParamType*)param);
	}

	template <typename FunctionInvokerType, typename ReturnType, typename ClassType, typename ParamType>
	void FunctionInvokerWrapper(void* functionInvoker, void* obj, void* rv, void* param)
	{
		if (rv != nullptr) {
			*((ReturnType*)rv) = REFL_INTERNAL_INVOKE(*(ParamType*)param);
		}
		else {
			REFL_INTERNAL_INVOKE(*(ParamType*)param);
		}
	}

	#undef REFL_INTERNAL_INVOKE
}