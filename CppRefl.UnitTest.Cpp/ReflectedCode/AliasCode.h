#pragma once

#include "CppReflMarkup.h"

#include "GeneratedCode/AliasCode.reflgen.h"



class REFLECTED ClassForAlias
{
	GENERATED_REFLECTION_CODE()
};

enum class REFLECTED EnumForAlias
{
};

template <typename T>
class REFLECTED TemplatedClassForAlias
{
};


class REFLECTED TemplatedSpecializationForAlias : public TemplatedClassForAlias<float>
{
	GENERATED_REFLECTION_CODE()
};


namespace TestNamespace
{
	class REFLECTED NamespacedClassForAlias {};
}

using IntAlias REFLECTED = int;
using ClassAlias REFLECTED = ClassForAlias;
using EnumAlias REFLECTED = EnumForAlias;
using AliasAlias REFLECTED = EnumAlias;

using TemplateAlias REFLECTED = TemplatedClassForAlias<int>; // Should reflected TemplatedClassForAlias<int> as class.
// TODO: Multiple aliases from the same class causes redefinitions of GetReflectedClass<>()
// using TemplateAliasAlias REFLECTED = TemplateAlias; // Should not reflect TemplateAlias as class.
using TemplateSpecializationAlias REFLECTED = TemplatedSpecializationForAlias; // Should reflect TemplatedSpecializationForAlias as class.

using NamespacedAlias REFLECTED = TestNamespace::NamespacedClassForAlias;
namespace TestNamespace
{
	using NamespacedAlias REFLECTED = NamespacedClassForAlias;
}