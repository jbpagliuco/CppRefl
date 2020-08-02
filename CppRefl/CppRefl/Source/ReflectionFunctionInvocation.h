#pragma once

#include "ReflectionTypes.h"

namespace refl
{
	template <typename ReturnType, typename ClassType, ReturnType (ClassType::*FunctionType)()>
	void FunctionInvokerTemplated(void* self, void *rv)
	{
		if (rv != nullptr) {
			*((ReturnType*)rv) = ((ClassType*)self->*FunctionType)();
		}
		else {
			((ClassType*)self->*FunctionType)();
		}
	}

	template <typename ClassType, void (ClassType::*FunctionType)()>
	void FunctionInvokerTemplatedVoid(void* self, void* dummy)
	{
		((ClassType*)self->*FunctionType)();
	}
}