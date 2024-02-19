#include "Registry.h"

namespace cpprefl
{
	Registry& Registry::GetSystemRegistry()
	{
		static Registry SystemRegistry;
		return SystemRegistry;
	}

	const TypeInfo& Registry::GetType(const Name& name)
	{
		return mTypes.at(name);
	}

	const ClassInfo& Registry::GetClass(const Name& name)
	{
		return mClasses.at(name);
	}

	const ClassInfo* Registry::TryGetClass(const Name& name)
	{
		if (mClasses.find(name) != mClasses.end())
		{
			return &GetClass(name);
		}

		return nullptr;
	}

	const EnumInfo& Registry::GetEnum(const Name& name)
	{
		return mEnums.at(name);
	}

	const EnumInfo* Registry::TryGetEnum(const Name& name)
	{
		if (mEnums.find(name) != mEnums.end())
		{
			return &GetEnum(name);
		}

		return nullptr;
	}

	const FunctionInfo& Registry::GetFunction(const Name& name)
	{
		return mFunctions.at(name);
	}

	const DynamicArrayFunctions& Registry::AddDynamicArrayFunctions(const Name& name, DynamicArrayFunctions functions)
	{
		if (mDynamicArrayFunctions.find(name) == mDynamicArrayFunctions.end())
		{
			return mDynamicArrayFunctions.emplace(name, functions).first->second;
		}

		return *GetDynamicArrayFunctions(name);
	}

	const DynamicArrayFunctions* Registry::GetDynamicArrayFunctions(const Name& name)
	{
		if (mDynamicArrayFunctions.find(name) != mDynamicArrayFunctions.end())
		{
			return &mDynamicArrayFunctions.at(name);
		}

		return nullptr;
	}

	Span<const ClassInfo*> Registry::GetDerivedClasses(const ClassInfo& baseClass) const
	{
		if (mClassHierarchy.find(&baseClass) != mClassHierarchy.end())
		{
			return Span(mClassHierarchy.at(&baseClass));
		}

		return {};
	}
}
