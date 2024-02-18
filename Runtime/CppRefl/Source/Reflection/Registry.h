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
	template <typename Key, typename Value>
	using RegistryHashMap = std::map<Key, Value>;

	// Contains all the reflected information in a program.
	class Registry
	{
	public:
		static Registry& GetSystemRegistry();

		TypeInfo& AddType(TypeInfo&& type);
		const TypeInfo& GetType(const Name& name);

		ClassInfo& AddClass(ClassInfo&& classInfo);
		const ClassInfo& GetClass(const Name& name);
		const ClassInfo* TryGetClass(const Name& name);

		EnumInfo& AddEnum(EnumInfo&& enumInfo);
		const EnumInfo& GetEnum(const Name& name);
		const EnumInfo* TryGetEnum(const Name& name);

		FunctionInfo& AddFunction(FunctionInfo&& functionInfo);
		const FunctionInfo& GetFunction(const Name& name);

		const DynamicArrayFunctions& AddDynamicArrayFunctions(const Name& name, const DynamicArrayFunctions& functions);
		const DynamicArrayFunctions* GetDynamicArrayFunctions(const Name& name);

		// Get a list of a derived classes.
		Span<const ClassInfo*> GetDerivedClasses(const ClassInfo& baseClass)const;

		template <typename T>
		Span<const ClassInfo*> GetDerivedClasses()const { return GetDerivedClasses(GetReflectedClass<T>()); }

	private:
		// Reflected types.
		RegistryHashMap<Name, TypeInfo> mTypes;

		// Reflected classes.
		RegistryHashMap<Name, ClassInfo> mClasses;

		// Reflected enums.
		RegistryHashMap<Name, EnumInfo> mEnums;

		// Reflected functions.
		RegistryHashMap<Name, FunctionInfo> mFunctions;

		// Dynamic array accessors.
		RegistryHashMap<Name, DynamicArrayFunctions> mDynamicArrayFunctions;

		// Base classes.
		RegistryHashMap<const ClassInfo*, std::vector<const ClassInfo*>> mClassHierarchy;
	};
}