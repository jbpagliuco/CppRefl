#pragma once

#include "ErrorHandling.h"
#include "ReflectionTypes.h"

namespace refl
{
	template <typename ReturnType, typename ClassType, ReturnType (ClassType::*FunctionType)()>
	void FunctionInvokerTemplated(void* obj, void *rv, void* dummyParam)
	{
		if (rv != nullptr) {
			*((ReturnType*)rv) = ((ClassType*)obj->*FunctionType)();
		}
		else {
			((ClassType*)obj->*FunctionType)();
		}
	}

	template <typename ReturnType, typename ClassType, typename ParamType, ReturnType (ClassType::*FunctionType)(ParamType)>
	void FunctionInvokerTemplatedParam(void* obj, void* rv, void* param)
	{
		if (param == nullptr) {
			REFL_RAISE_ERROR_INTERNAL("Tried to invoke a function with a null parameter.");
			return;
		}

		if (rv != nullptr) {
			*((ReturnType*)rv) = ((ClassType*)obj->*FunctionType)(*(ParamType*)param);
		}
		else {
			((ClassType*)obj->*FunctionType)(*(ParamType*)param);
		}
	}

	template <typename ClassType, void (ClassType::*FunctionType)()>
	void FunctionInvokerTemplatedVoid(void* obj, void* dummyRV, void* dummyParam)
	{
		((ClassType*)obj->*FunctionType)();
	}

	template <typename ClassType, typename ParamType, void (ClassType::*FunctionType)(ParamType)>
	void FunctionInvokerTemplatedVoidParam(void* obj, void* dummyRV, void* param)
	{
		if (param == nullptr) {
			//RAISE_ERROR
			return;
		}

		((ClassType*)obj->*FunctionType)(*(ParamType*)param);
	}
}