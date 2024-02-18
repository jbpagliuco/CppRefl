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
		std::map<Name, TypeInfo> mTypes;

		// Reflected classes.
		std::map<Name, ClassInfo> mClasses;

		// Reflected enums.
		std::map<Name, EnumInfo> mEnums;

		// Reflected functions.
		std::map<Name, FunctionInfo> mFunctions;

		// Dynamic array accessors.
		std::map<Name, DynamicArrayFunctions> mDynamicArrayFunctions;

		// Base classes.
		std::map<const ClassInfo*, std::vector<const ClassInfo*>> mClassHierarchy;
	};
}