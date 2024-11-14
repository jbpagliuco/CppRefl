#pragma once

#include <type_traits>

#include "CppReflConfig.h"

#if CPPREFL_WITH_STL()
#include <string>
#endif

#include "Reflection/TypeInfo.h"
#include "CppReflConfig.h"
#include "CppReflHash.h"

namespace CppReflPrivate
{
	// Creates a static type if it doesn't already exist.
	template <typename T>
	const cpprefl::TypeInfo& MaybeCreateReflectedType(const cpprefl::Name& typeName);
}

namespace cpprefl
{
	class ClassInfo;
	class EnumInfo;
	class FunctionInfo;

	// Returns a static type known at compile time.
	template <typename T>
	const TypeInfo& GetReflectedType() = delete;

	// Static types for primitive types.
	template <> inline const TypeInfo& GetReflectedType<bool>()					{ return CppReflPrivate::MaybeCreateReflectedType<bool>(EnsureName("bool")); }
	template <> inline const TypeInfo& GetReflectedType<unsigned char>()		{ return CppReflPrivate::MaybeCreateReflectedType<unsigned char>(EnsureName("unsigned char")); }
	template <> inline const TypeInfo& GetReflectedType<char>()					{ return CppReflPrivate::MaybeCreateReflectedType<char>(EnsureName("char")); }
	template <> inline const TypeInfo& GetReflectedType<unsigned short>()		{ return CppReflPrivate::MaybeCreateReflectedType<unsigned short>(EnsureName("unsigned short")); }
	template <> inline const TypeInfo& GetReflectedType<short>()				{ return CppReflPrivate::MaybeCreateReflectedType<short>(EnsureName("short")); }
	template <> inline const TypeInfo& GetReflectedType<unsigned int>()			{ return CppReflPrivate::MaybeCreateReflectedType<unsigned int>(EnsureName("unsigned int")); }
	template <> inline const TypeInfo& GetReflectedType<int>()					{ return CppReflPrivate::MaybeCreateReflectedType<int>(EnsureName("int")); }
	template <> inline const TypeInfo& GetReflectedType<unsigned long>()		{ return CppReflPrivate::MaybeCreateReflectedType<unsigned long>(EnsureName("unsigned long")); }
	template <> inline const TypeInfo& GetReflectedType<long>()					{ return CppReflPrivate::MaybeCreateReflectedType<long>(EnsureName("long")); }
	template <> inline const TypeInfo& GetReflectedType<unsigned long long>()	{ return CppReflPrivate::MaybeCreateReflectedType<unsigned long long>(EnsureName("unsigned long long")); }
	template <> inline const TypeInfo& GetReflectedType<long long>()			{ return CppReflPrivate::MaybeCreateReflectedType<long long>(EnsureName("long long")); }
	template <> inline const TypeInfo& GetReflectedType<float>()				{ return CppReflPrivate::MaybeCreateReflectedType<float>(EnsureName("float")); }
	template <> inline const TypeInfo& GetReflectedType<double>()				{ return CppReflPrivate::MaybeCreateReflectedType<double>(EnsureName("double")); }
	template <> inline const TypeInfo& GetReflectedType<long double>()			{ return CppReflPrivate::MaybeCreateReflectedType<long double>(EnsureName("long double")); }
	template <> inline const TypeInfo& GetReflectedType<void>()					{ return CppReflPrivate::MaybeCreateReflectedType<void>(EnsureName("void")); }

#if CPPREFL_WITH_STL()
	template <> inline const TypeInfo& GetReflectedType<std::string>()			{ return CppReflPrivate::MaybeCreateReflectedType<std::string>(EnsureName("std::string")); }
#endif

	// Returns a static enum known at compile time.
	template <typename T>
	const EnumInfo& GetReflectedEnum() = delete;

	// Returns a static class known at compile time.
	template <typename T>
	const ClassInfo& GetReflectedClass() = delete;

	// Returns a static function known at compile time.
	template <void* FunctionAddress>
	const FunctionInfo& GetReflectedFunction() = delete;

	// Returns the name of a reflected class.
	template <typename T>
	Name GetTypeName()
	{
		return GetReflectedType<T>().mName;
	}

	// Returns true if the given type represents the template parameter.
	template <typename T>
	bool IsSameType(const TypeInfo& type)
	{
		return &type == &GetReflectedType<T>();
	}

#if CPPREFL_CONCEPTS()
	template <typename T>
	concept ReflectedType = requires(T t)
	{
		GetReflectedType<T>();
	};

	template <typename T>
	concept ReflectedClass = requires(T t)
	{
		GetReflectedClass<T>();
	};

	template <typename T>
	concept ReflectedEnum = requires(T t)
	{
		GetReflectedEnum<T>();
	};
#else
	template <typename T, typename = int> struct IsReflectedType : std::false_type {};
	template <typename T> struct IsReflectedType<T, decltype((void)GetReflectedType<T>(), 0)> : std::true_type {};
	template <typename T> constexpr bool IsReflectedType_v = IsReflectedType<T>::value;

	template <typename T, typename = int> struct IsReflectedClass : std::false_type {};
	template <typename T> struct IsReflectedClass<T, decltype((void)GetReflectedClass<T>(), 0)> : std::true_type {};
	template <typename T> constexpr bool IsReflectedClass_v = IsReflectedClass<T>::value;

	template <typename T, typename = int> struct IsReflectedEnum : std::false_type {};
	template <typename T> struct IsReflectedEnum<T, decltype((void)GetReflectedEnum<T>(), 0)> : std::true_type {};
	template <typename T> constexpr bool IsReflectedEnum_v = IsReflectedEnum<T>::value;
#endif
}

namespace CppReflPrivate
{
	template <typename T>
	const cpprefl::TypeInfo& MaybeCreateReflectedType(const cpprefl::Name& typeName)
	{
#if CPPREFL_WITH_STL()
		if constexpr (std::is_same_v<T, std::string>)
		{
			static cpprefl::TypeInfo staticType(typeName, cpprefl::TypeKind::Class, sizeof(T));
			return staticType;
		}
#endif

#if CPPREFL_CONCEPTS()
		if constexpr (std::is_class_v<T> && cpprefl::ReflectedType<T>)
		{
			return cpprefl::GetReflectedType<T>();
		}
#else
		if constexpr (std::is_class_v<T> && cpprefl::IsReflectedType_v<T>)
		{
			return cpprefl::GetReflectedType<T>();
		}
#endif
		else if constexpr (std::is_void_v<T>)
		{
			static cpprefl::TypeInfo staticType(typeName, cpprefl::TypeKind::Void, 0);
			return staticType;
		}
		else
		{
			static cpprefl::TypeInfo staticType(typeName, cpprefl::GetTypeKind<T>(), sizeof(T));
			return staticType;
		}
	}
}