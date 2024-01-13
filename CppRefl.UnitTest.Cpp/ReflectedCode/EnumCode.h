#pragma once

#include "CppReflMarkup.h"

#include "GeneratedCode/EnumCode.reflgen.h"

enum class REFLECTED ReflectedEnum
{
	EnumValue1 = 0,
	Bar = 10,
	Foo,
};

namespace TestNamespace
{
	enum class REFLECTED NamespacedEnum
	{
		Valu1,
		Valu2
	};
}