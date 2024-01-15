#pragma once

#include <vector>

#include "CppReflMarkup.h"

#include "TemplateCode.reflgen.h"

namespace TestNamespace
{
	// Template base class.
	template <typename T>
	class REFLECTED TemplatedClass
	{
		// GENERATED_REFLECTION_CODE()

	public:
		T mValue REFLECTED;
	};

	// Secondary template base class.
	template <typename T>
	class REFLECTED TemplatedClass2 : public TemplatedClass<T>
	{};

	// Template class that implicitly declares TemplatedClass2<float>.
	class REFLECTED TemplatedClassSpecialization : TemplatedClass2<float>
	{
		GENERATED_REFLECTION_CODE()

	public:
		bool mNonTemplatedField REFLECTED;
	};
}

namespace TestNamespace
{
	class REFLECTED NamespacedClass
	{};
}
namespace TestNamespace2
{
	class REFLECTED NamespacedClass
	{};
}

using namespace TestNamespace2;

// Ensure that namespaces work within field types.
class REFLECTED NonTemplatedClass
{
	GENERATED_REFLECTION_CODE()

public:
	std::vector<int> mArray REFLECTED;
	std::vector<TestNamespace::NamespacedClass> mNamespacedArray REFLECTED;
	std::vector<NamespacedClass> mNamespacedArray2 REFLECTED;
};