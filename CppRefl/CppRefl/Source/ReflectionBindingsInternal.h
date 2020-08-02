#pragma once

namespace refl
{
#define REFL_BIND_FUNCTION_RV_INTERNAL2(ReturnType, ClassName, Function, Line) \
	static refl::FunctionRegistration _##Function##Line(#ClassName, #Function, &refl::FunctionInvokerTemplated<ReturnType, ClassName, &ClassName::Function>);
#define REFL_BIND_FUNCTION_RV_INTERNAL(ReturnType, ClassName, Function, Line) REFL_BIND_FUNCTION_RV_INTERNAL2(ReturnType, ClassName, Function, Line)

#define REFL_BIND_FUNCTION_INTERNAL2(ClassName, Function, Line) \
	static refl::FunctionRegistration _##Function##Line(#ClassName, #Function, &refl::FunctionInvokerTemplatedVoid<ClassName, &ClassName::Function>);
#define REFL_BIND_FUNCTION_INTERNAL(ClassName, Function, Line) REFL_BIND_FUNCTION_INTERNAL2(ClassName, Function, Line)
}