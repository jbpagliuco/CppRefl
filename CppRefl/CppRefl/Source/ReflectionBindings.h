#pragma once

#include "ReflectionBindingsInternal.h"

// Binds and registers a reflected member function with no return value with the reflection system.
// ClassName: The fully qualified name of the class that contains the function to be bound.
// Function: The function name to be bound.
// ...: Variadic list of parameter types.
#define REFL_BIND_VOID_FUNCTION(ClassName, Function, ...)				REFL_INTERNAL_BIND_VOID_FUNCTION(ClassName, Function, __VA_ARGS__);

// Binds and registers a reflected member function that returns a value with the reflection system.
// ClassName: The fully qualified name of the class that contains the function to be bound.
// Function: The function name to be bound.
// ...: Variadic list of parameter types.
#define REFL_BIND_FUNCTION(ReturnType, ClassName, Function, ...)		REFL_INTERNAL_BIND_FUNCTION(ReturnType, ClassName, Function, __VA_ARGS__);


// Binds and registers a reflected global function with no return value with the reflection system.
// Function: The function name to be bound.
// ...: Variadic list of parameter types.
#define REFL_BIND_VOID_GLOBAL_FUNCTION(Function, ...)							REFL_INTERNAL_BIND_VOID_GLOBAL_FUNCTION(, Function, __VA_ARGS__);

// Binds and registers a reflected global function that returns a value with the reflection system.
// Function: The function name to be bound.
// ...: Variadic list of parameter types.
#define REFL_BIND_GLOBAL_FUNCTION(ReturnType, Function, ...)					REFL_INTERNAL_BIND_GLOBAL_FUNCTION(, ReturnType, Function, __VA_ARGS__);

// Binds and registers a reflected global function with no return value with the reflection system.
// Namespace: The namespace that contains the function to be bound.
// Function: The function name to be bound.
// ...: Variadic list of parameter types.
#define REFL_BIND_VOID_GLOBAL_FUNCTION_NS(Namespace, Function, ...)				REFL_INTERNAL_BIND_VOID_GLOBAL_FUNCTION(Namespace "::", Function, __VA_ARGS__);

// Binds and registers a reflected global function that returns a value with the reflection system.
// Namespace: The namespace that contains the function to be bound.
// Function: The function name to be bound.
// ...: Variadic list of parameter types.
#define REFL_BIND_GLOBAL_FUNCTION_NS(ReturnType, Namespace, Function, ...)		REFL_INTERNAL_BIND_GLOBAL_FUNCTION(Namespace "::", ReturnType, Function, __VA_ARGS__);