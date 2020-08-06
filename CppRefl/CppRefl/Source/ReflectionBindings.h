#pragma once

#include "ReflectionBindingsInternal.h"
#include "ReflectionFunctionInvocation.h"

// Registers a reflected function with no return value.
#define REFL_BIND_VOID_FUNCTION(ClassName, Function, ...)				REFL_INTERNAL_BIND_VOID_FUNCTION(ClassName, Function, __VA_ARGS__);

// Registers a reflected function that returns a value.
#define REFL_BIND_FUNCTION(ReturnType, ClassName, Function, ...)		REFL_INTERNAL_BIND_FUNCTION(ReturnType, ClassName, Function, __VA_ARGS__);