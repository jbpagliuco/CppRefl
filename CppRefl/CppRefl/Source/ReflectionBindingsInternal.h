#pragma once

// One parameter, no return value
#define REFL_BIND_FUNCTION_INTERNAL2(ClassName, Function, Line) \
	static refl::FunctionRegistration _##Function##Line(#ClassName, #Function, &refl::FunctionInvokerTemplatedVoid<ClassName, &ClassName::Function>);
#define REFL_BIND_FUNCTION_INTERNAL(ClassName, Function, Line) REFL_BIND_FUNCTION_INTERNAL2(ClassName, Function, Line)

// No parameter, no return value
#define REFL_BIND_FUNCTION_PARAM_INTERNAL2(ClassName, Function, ParamType, Line) \
	static refl::FunctionRegistration _##Function##Line(#ClassName, #Function, &refl::FunctionInvokerTemplatedVoidParam<ClassName, ParamType, &ClassName::Function>);
#define REFL_BIND_FUNCTION_PARAM_INTERNAL(ClassName, Function, ParamType, Line) REFL_BIND_FUNCTION_PARAM_INTERNAL2(ClassName, Function, ParamType, Line)

// No parameter, one return value
#define REFL_BIND_FUNCTION_RV_INTERNAL2(ReturnType, ClassName, Function, Line) \
	static refl::FunctionRegistration _##Function##Line(#ClassName, #Function, &refl::FunctionInvokerTemplated<ReturnType, ClassName, &ClassName::Function>);
#define REFL_BIND_FUNCTION_RV_INTERNAL(ReturnType, ClassName, Function, Line) REFL_BIND_FUNCTION_RV_INTERNAL2(ReturnType, ClassName, Function, Line)

// One parameter, one return value
#define REFL_BIND_FUNCTION_RV_PARAM_INTERNAL2(ReturnType, ClassName, Function, ParamType, Line) \
	static refl::FunctionRegistration _##Function##Line(#ClassName, #Function, &refl::FunctionInvokerTemplatedParam<ReturnType, ClassName, ParamType, &ClassName::Function>);
#define REFL_BIND_FUNCTION_RV_PARAM_INTERNAL(ReturnType, ClassName, Function, ParamType, Line) REFL_BIND_FUNCTION_RV_PARAM_INTERNAL2(ReturnType, ClassName, Function, ParamType, Line)
