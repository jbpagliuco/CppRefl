#pragma once

#include "ReflectionBindingsInternal.h"
#include "ReflectionFunctionInvocation.h"

// Binds and registers a reflected function with no return value with the reflection system.
#define REFL_BIND_VOID_FUNCTION(ClassName, Function, ...)				REFL_INTERNAL_BIND_VOID_FUNCTION(ClassName, Function, __VA_ARGS__);

// Binds and registers a reflected function that returns a value with the reflection system.
#define REFL_BIND_FUNCTION(ReturnType, ClassName, Function, ...)		REFL_INTERNAL_BIND_FUNCTION(ReturnType, ClassName, Function, __VA_ARGS__);