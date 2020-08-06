#pragma once

#include "ErrorHandling.h"
#include "ReflectionTypes.h"

namespace refl
{
	// FunctionInvoker is a wrapper class that can invoke a function with any number of parameters.
	template<typename T, T> struct FunctionInvoker;

	// Function invoker for a member function of a class.
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


	// Helper define to invoke a member function.
	#define REFL_INTERNAL_INVOKE_MEMBER(...) static_cast<FunctionInvokerType*>(functionInvoker)->Invoke((ClassType*)obj, __VA_ARGS__)

	// Invokes a member function with no return value and no parameters.
	template <typename FunctionInvokerType, typename ClassType>
	void VoidMemberFunctionInvokerWrapper(void* functionInvoker, void* obj, void* rv, void* dummyParam)
	{
		REFL_INTERNAL_INVOKE_MEMBER();
	}

	// Invokes a member function with a return value and no parameters.
	template <typename FunctionInvokerType, typename ReturnType, typename ClassType>
	void MemberFunctionInvokerWrapper(void* functionInvoker, void* obj, void* rv, void* dummyParam)
	{
		if (rv != nullptr) {
			*((ReturnType*)rv) = REFL_INTERNAL_INVOKE_MEMBER();
		}
		else {
			REFL_INTERNAL_INVOKE_MEMBER();
		}
	}
	
	// Invokes a member function with no return value and one parameter.
	template <typename FunctionInvokerType, typename ClassType, typename ParamType>
	void VoidMemberFunctionInvokerWrapper(void* functionInvoker, void* obj, void* rv, void* param)
	{
		REFL_INTERNAL_INVOKE_MEMBER(*(ParamType*)param);
	}

	// Invokes a member function with a return value and one parameter.
	template <typename FunctionInvokerType, typename ReturnType, typename ClassType, typename ParamType>
	void MemberFunctionInvokerWrapper(void* functionInvoker, void* obj, void* rv, void* param)
	{
		if (rv != nullptr) {
			*((ReturnType*)rv) = REFL_INTERNAL_INVOKE_MEMBER(*(ParamType*)param);
		}
		else {
			REFL_INTERNAL_INVOKE_MEMBER(*(ParamType*)param);
		}
	}

	#undef REFL_INTERNAL_INVOKE_MEMBER
}