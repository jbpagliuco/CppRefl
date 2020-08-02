#pragma once

#include "ReflectionBindingsInternal.h"
#include "ReflectionFunctionInvocation.h"

// Registers a void function with the reflection system.
#define REFL_BIND_FUNCTION(ClassName, Function)						REFL_BIND_FUNCTION_INTERNAL(ClassName, Function, __LINE__);

// Registers a function that returns a value with the reflection system.
#define REFL_BIND_FUNCTION_RV(ReturnType, ClassName, Function)		REFL_BIND_FUNCTION_RV_INTERNAL(ReturnType, ClassName, Function, __LINE__);
