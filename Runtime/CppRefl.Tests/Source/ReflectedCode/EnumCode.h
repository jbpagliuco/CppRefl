#pragma once

#define TEST_ENUM_CODE() 1

#if TEST_ENUM_CODE()

#include "CppReflMarkup.h"

#include "EnumCode.reflgen.h"

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

#endif