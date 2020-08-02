#pragma once

#include "ReflectionTypes.h"

namespace refl
{
	template <typename ReturnType, typename ClassType, ReturnType (ClassType::*FunctionType)()>
	void FunctionInvokerTemplated(void* self, void *rv, void* dummyParam)
	{
		if (rv != nullptr) {
			*((ReturnType*)rv) = ((ClassType*)self->*FunctionType)();
		}
		else {
			((ClassType*)self->*FunctionType)();
		}
	}

	template <typename ReturnType, typename ClassType, typename ParamType, ReturnType (ClassType::*FunctionType)(ParamType*)>
	void FunctionInvokerTemplatedParam(void* self, void* rv, void* param)
	{
		if (rv != nullptr) {
			*((ReturnType*)rv) = ((ClassType*)self->*FunctionType)((ParamType*)param);
		}
		else {
			((ClassType*)self->*FunctionType)((ParamType*)param);
		}
	}

	template <typename ClassType, void (ClassType::*FunctionType)()>
	void FunctionInvokerTemplatedVoid(void* self, void* dummyRV, void* dummyParam)
	{
		((ClassType*)self->*FunctionType)();
	}

	template <typename ClassType, typename ParamType, void (ClassType::*FunctionType)(ParamType*)>
	void FunctionInvokerTemplatedVoidParam(void* self, void* dummyRV, void* param)
	{
		((ClassType*)self->*FunctionType)((ParamType*)param);
	}
}