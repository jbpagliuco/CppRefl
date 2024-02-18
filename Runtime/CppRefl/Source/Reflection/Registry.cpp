#include "Registry.h"

namespace cpprefl
{
	Registry& Registry::GetSystemRegistry()
	{
		static Registry SystemRegistry;
		return SystemRegistry;
	}

	TypeInfo& Registry::AddType(TypeInfo&& type)
	{
		return mTypes.emplace(type.mName, type).first->second;
	}

	const TypeInfo& Registry::GetType(const Name& name)
	{
		return mTypes.at(name);
	}

	const ClassInfo& Registry::AddClass(ClassInfo&& inClassInfo)
	{
		auto& classInfo = mClasses.emplace(inClassInfo.mType->mName, inClassInfo).first->second;

		// Update the class heirarchy.
		mClassHierarchy[&classInfo] = {};

		// Update all base classes.
		const ClassInfo* baseClass = classInfo.mBaseClass;
		while (baseClass != nullptr)
		{
			if (mClassHierarchy.find(baseClass) == mClassHierarchy.end())
			{
				mClassHierarchy[baseClass] = { &classInfo };
			}
			else
			{
				mClassHierarchy[baseClass].push_back(&classInfo);
			}

			baseClass = baseClass->mBaseClass;
		}


		return classInfo;
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

	const EnumInfo& Registry::AddEnum(EnumInfo&& enumInfo)
	{
		return mEnums.emplace(enumInfo.mType->mName, enumInfo).first->second;
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

	const FunctionInfo& Registry::AddFunction(FunctionInfo&& functionInfo)
	{
		return mFunctions.emplace(functionInfo.mName, functionInfo).first->second;
	}

	const FunctionInfo& Registry::GetFunction(const Name& name)
	{
		return mFunctions.at(name);
	}

	const DynamicArrayFunctions& Registry::AddDynamicArrayFunctions(const Name& name, const DynamicArrayFunctions& functions)
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
