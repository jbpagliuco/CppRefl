#pragma once

#define REFL_BIND_GEN_NAME2(FunctionName, Line)		_##FunctionName##Line
#define REFL_BIND_GEN_NAME1(FunctionName, Line)		REFL_BIND_GEN_NAME2(FunctionName, Line)
#define REFL_BIND_GEN_NAME(FunctionName, Desc)		REFL_BIND_GEN_NAME1(FunctionName ## Desc, __LINE__)



#define REFL_INTERNAL_BIND_VOID_FUNCTION(ClassName, FunctionName, ...)																				\
	static refl::FunctionInvoker<void (ClassName::*)(__VA_ARGS__), &ClassName::FunctionName> REFL_BIND_GEN_NAME(FunctionName, Invoker);				\
	static refl::FunctionRegistration REFL_BIND_GEN_NAME(FunctionName, Wrapper)(#ClassName, #FunctionName, &refl::VoidFunctionInvokerWrapper<decltype(REFL_BIND_GEN_NAME(FunctionName, Invoker)), ClassName, __VA_ARGS__>, &REFL_BIND_GEN_NAME(FunctionName, Invoker));


#define REFL_INTERNAL_BIND_FUNCTION(ReturnType, ClassName, FunctionName, ...)																				\
	static refl::FunctionInvoker<ReturnType (ClassName::*)(__VA_ARGS__), &ClassName::FunctionName> REFL_BIND_GEN_NAME(FunctionName, Invoker);				\
	static refl::FunctionRegistration REFL_BIND_GEN_NAME(FunctionName, Wrapper)(#ClassName, #FunctionName, &refl::FunctionInvokerWrapper<decltype(REFL_BIND_GEN_NAME(FunctionName, Invoker)), ReturnType, ClassName, __VA_ARGS__>, &REFL_BIND_GEN_NAME(FunctionName, Invoker));
