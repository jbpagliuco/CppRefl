#pragma once

#include <map>
#include <string>

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
		const TypeInfo& GetType(const char* name);

		ClassInfo& AddClass(ClassInfo&& classInfo);
		const ClassInfo& GetClass(const char* name);
		const ClassInfo* TryGetClass(const char* name);

		EnumInfo& AddEnum(EnumInfo&& enumInfo);
		const EnumInfo& GetEnum(const char* name);
		const EnumInfo* TryGetEnum(const char* name);

		FunctionInfo& AddFunction(FunctionInfo&& functionInfo);
		const FunctionInfo& GetFunction(const char* name);

		const DynamicArrayFunctions& AddDynamicArrayFunctions(const char* name, const DynamicArrayFunctions& functions);
		const DynamicArrayFunctions* GetDynamicArrayFunctions(const char* name);

		// Get a list of a derived classes.
		Span<const ClassInfo*> GetDerivedClasses(const ClassInfo& baseClass)const;

		template <typename T>
		Span<const ClassInfo*> GetDerivedClasses()const { return GetDerivedClasses(GetReflectedClass<T>()); }

	private:
		// Reflected types.
		std::map<std::string, TypeInfo> mTypes;

		// Reflected classes.
		std::map<std::string, ClassInfo> mClasses;

		// Reflected enums.
		std::map<std::string, EnumInfo> mEnums;

		// Reflected functions.
		std::map<std::string, FunctionInfo> mFunctions;

		// Dynamic array accessors.
		std::map<std::string, DynamicArrayFunctions> mDynamicArrayFunctions;

		// Base classes.
		std::map<const ClassInfo*, std::vector<const ClassInfo*>> mClassHierarchy;
	};
}