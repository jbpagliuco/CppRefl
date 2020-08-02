#pragma once

#include "ReflectionBindingsInternal.h"
#include "ReflectionFunctionInvocation.h"

// Registers a reflected function with no parameters and no return value.
#define REFL_BIND_FUNCTION(ClassName, Function)						REFL_BIND_FUNCTION_INTERNAL(ClassName, Function, __LINE__);

// Registers a reflected function with one parameter and no return value.
#define REFL_BIND_FUNCTION_PARAM(ClassName, Function, ParamType)	REFL_BIND_FUNCTION_PARAM_INTERNAL(ClassName, Function, ParamType, __LINE__);

// Registers a reflected function with no parameters and a return value.
#define REFL_BIND_FUNCTION_RV(ReturnType, ClassName, Function)		REFL_BIND_FUNCTION_RV_INTERNAL(ReturnType, ClassName, Function, __LINE__);

// Registers a reflected function with one parameter and a return value.
#define REFL_BIND_FUNCTION_RV_PARAM(ReturnType, ClassName, Function, ParamType)		REFL_BIND_FUNCTION_RV_PARAM_INTERNAL(ReturnType, ClassName, Function, ParamType, __LINE__);