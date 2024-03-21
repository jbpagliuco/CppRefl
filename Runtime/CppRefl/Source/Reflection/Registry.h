#pragma once

#include <map>

#include "ClassInfo.h"
#include "DynamicArray.h"
#include "EnumInfo.h"
#include "FunctionInfo.h"
#include "TypeInfo.h"
#include "../CppReflStatics.h"

namespace cpprefl
{
	// Contains all the reflected information in a program.
	class Registry
	{
	public:
		static Registry& GetSystemRegistry();

		template <typename... Params>
		const TypeInfo& EmplaceType(Name name, Params&&... params);
		const TypeInfo& GetType(const Name& name);

		template <typename... Params>
		const ClassInfo& EmplaceClass(const TypeInfo* typeInfo, Params&&... params);
		const ClassInfo& GetClass(const Name& name);
		const ClassInfo* TryGetClass(const Name& name);

		template <typename... Params>
		const EnumInfo& EmplaceEnum(const TypeInfo* typeInfo, Params&&... params);
		const EnumInfo& GetEnum(const Name& name);
		const EnumInfo* TryGetEnum(const Name& name);

		template <typename... Params>
		const FunctionInfo& EmplaceFunction(Name name, Params&&... params);
		const FunctionInfo& GetFunction(const Name& name);

		const DynamicArrayFunctions& AddDynamicArrayFunctions(const Name& name, DynamicArrayFunctions functions);
		const DynamicArrayFunctions* GetDynamicArrayFunctions(const Name& name);

		// Get a list of a derived classes.
		Span<const ClassInfo*> GetDerivedClasses(const ClassInfo& baseClass)const;

		template <typename T>
		Span<const ClassInfo*> GetDerivedClasses()const { return GetDerivedClasses(GetReflectedClass<T>()); }

	private:
		// Reflected types.
		HashMap<Name, TypeInfo> mTypes;

		// Reflected classes.
		HashMap<Name, ClassInfo> mClasses;

		// Reflected enums.
		HashMap<Name, EnumInfo> mEnums;

		// Reflected functions.
		HashMap<Name, FunctionInfo> mFunctions;

		// Dynamic array accessors.
		HashMap<Name, DynamicArrayFunctions> mDynamicArrayFunctions;

		// Base classes.
		HashMap<const ClassInfo*, std::vector<const ClassInfo*>> mClassHierarchy;
	};

	template <typename ... Params>
	const TypeInfo& Registry::EmplaceType(Name name, Params&&... params)
	{
#if CPPREFL_DEBUG()
		// Ensure this type has is unique.
		if (mTypes.find(name) != mTypes.end())
		{
			const auto& existingTypeInfo = mTypes.at(name);

#if CPPREFL_STORE_NAMES()
			CPPREFL_INTERNAL_LOG(LogLevel::Error, "Tried to add the same type '%s' twice. How did this happen?", name.GetString());
#else
			CPPREFL_INTERNAL_LOG(LogLevel::Error, "Tried to add the same type (hash ='%zu') twice. How did this happen?", name.mHash);
#endif
		}
#endif

		return mTypes.try_emplace(name, name, std::forward<Params>(params)...).first->second;
	}

	template <typename ... Params>
	const ClassInfo& Registry::EmplaceClass(const TypeInfo* typeInfo, Params&&... params)
	{
		auto& classInfo = mClasses.try_emplace(typeInfo->mName, typeInfo, std::forward<Params>(params)...).first->second;

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

	template <typename ... Params>
	const EnumInfo& Registry::EmplaceEnum(const TypeInfo* typeInfo, Params&&... params)
	{
		return mEnums.try_emplace(typeInfo->mName, typeInfo, std::forward<Params>(params)...).first->second;
	}

	template <typename ... Params>
	const FunctionInfo& Registry::EmplaceFunction(Name name, Params&&... params)
	{
		return mFunctions.try_emplace(name, name, std::forward<Params>(params)...).first->second;
	}
}
