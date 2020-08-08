#pragma once

#include "ReflectionFunctionInvocation.h"
#include "ReflectionFunctionRegistration.h"


// Generates a (mostly) unique variable/function name.
#define REFL_BIND_GEN_NAME2(FunctionName, Line)		_##FunctionName##Line
#define REFL_BIND_GEN_NAME1(FunctionName, Line)		REFL_BIND_GEN_NAME2(FunctionName, Line)
#define REFL_BIND_GEN_NAME(FunctionName, Desc)		REFL_BIND_GEN_NAME1(FunctionName ## Desc, __LINE__)


// Bind and register a member function with no return value.
#define REFL_INTERNAL_BIND_VOID_FUNCTION(ClassName, FunctionName, ...)																				\
	static refl::FunctionInvoker<void (ClassName::*)(__VA_ARGS__), &ClassName::FunctionName> REFL_BIND_GEN_NAME(FunctionName, Invoker);				\
	static refl::FunctionRegistration REFL_BIND_GEN_NAME(FunctionName, Wrapper)(#ClassName, #FunctionName, &refl::VoidMemberFunctionInvokerWrapper<decltype(REFL_BIND_GEN_NAME(FunctionName, Invoker)), ClassName, __VA_ARGS__>, &REFL_BIND_GEN_NAME(FunctionName, Invoker));


// Bind and register a member function with a return value.
#define REFL_INTERNAL_BIND_FUNCTION(ReturnType, ClassName, FunctionName, ...)																				\
	static refl::FunctionInvoker<ReturnType (ClassName::*)(__VA_ARGS__), &ClassName::FunctionName> REFL_BIND_GEN_NAME(FunctionName, Invoker);				\
	static refl::FunctionRegistration REFL_BIND_GEN_NAME(FunctionName, Wrapper)(#ClassName, #FunctionName, &refl::MemberFunctionInvokerWrapper<decltype(REFL_BIND_GEN_NAME(FunctionName, Invoker)), ReturnType, ClassName, __VA_ARGS__>, &REFL_BIND_GEN_NAME(FunctionName, Invoker));



// Bind and register a global function with no return value.
// NB: The Namespace parameter should have a trailing :: passed in.
#define REFL_INTERNAL_BIND_VOID_GLOBAL_FUNCTION(Namespace, FunctionName, ...)															\
	static refl::FunctionInvoker<void (*)(__VA_ARGS__), &Namespace FunctionName> REFL_BIND_GEN_NAME(FunctionName, Invoker);			\
	static refl::FunctionRegistration REFL_BIND_GEN_NAME(FunctionName, Wrapper)(#Namespace #FunctionName, &refl::VoidGlobalFunctionInvokerWrapper<decltype(REFL_BIND_GEN_NAME(FunctionName, Invoker)), __VA_ARGS__>, &REFL_BIND_GEN_NAME(FunctionName, Invoker));


// Bind and register a global function with no return value.
// NB: The Namespace parameter should have a trailing :: passed in.
#define REFL_INTERNAL_BIND_GLOBAL_FUNCTION(Namespace, ReturnType, FunctionName, ...)													\
	static refl::FunctionInvoker<ReturnType (*)(__VA_ARGS__), &Namespace FunctionName> REFL_BIND_GEN_NAME(FunctionName, Invoker);		\
	static refl::FunctionRegistration REFL_BIND_GEN_NAME(FunctionName, Wrapper)(#Namespace #FunctionName, &refl::GlobalFunctionInvokerWrapper<decltype(REFL_BIND_GEN_NAME(FunctionName, Invoker)), ReturnType, __VA_ARGS__>, &REFL_BIND_GEN_NAME(FunctionName, Invoker));

