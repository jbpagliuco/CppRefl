#pragma once

#include <cassert>
#include <type_traits>

#include "Reflection/TypeInfo.h"

#if __cplusplus >= 202002L
#define CPPREFL_CONCEPTS() 1
#else
#define CPPREFL_CONCEPTS() 0
#endif

namespace CppReflPrivate
{
	// Helper to figure out if a template parameter has a member variable named "StaticType".
	template <typename T, typename = int>
	struct HasStaticType : std::false_type { };

	// Helper to figure out if a template parameter has a member variable named "StaticType".
	template <typename T>
	struct HasStaticType<T, decltype((void)T::StaticType, 0)> : std::true_type { };

	// Creates a static type if it doesn't already exist.
	template <typename T>
	const cpprefl::TypeInfo& MaybeCreateStaticType(const char* typeName)
	{
		if constexpr (std::is_class_v<T> && HasStaticType<T>::value)
		{
			return T::StaticType();
		}
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

namespace cpprefl
{
	class EnumInfo;
	class FunctionInfo;

	// Returns a static type known at compile time.
	template <typename T>
	const TypeInfo& StaticType();

	// Returns a static type known at compile-time.
	template <typename T>
	const TypeInfo& StaticType()
	{
		if constexpr (!std::is_class_v<T> && !std::is_enum_v<T>)
		{
			const TypeInfo& type = CppReflPrivate::MaybeCreateStaticType<T>("");
			assert(type.mName[0] != 0 && "Type info was never created. This is illegal.");

			return type;
		}

		static_assert(!std::is_class_v<T>, "This class doesn't implement StaticType<>()");
		static_assert(!std::is_enum_v<T>, "This enum doesn't implement StaticEnum<>()");
	}

	// Static types for primitive types.
	template <> inline const TypeInfo& StaticType<bool>()				{ return CppReflPrivate::MaybeCreateStaticType<bool>("bool"); }
	template <> inline const TypeInfo& StaticType<unsigned char>()		{ return CppReflPrivate::MaybeCreateStaticType<unsigned char>("unsigned char"); }
	template <> inline const TypeInfo& StaticType<char>()				{ return CppReflPrivate::MaybeCreateStaticType<char>("char"); }
	template <> inline const TypeInfo& StaticType<unsigned short>()		{ return CppReflPrivate::MaybeCreateStaticType<unsigned short>("unsigned short"); }
	template <> inline const TypeInfo& StaticType<short>()				{ return CppReflPrivate::MaybeCreateStaticType<short>("short"); }
	template <> inline const TypeInfo& StaticType<unsigned int>()		{ return CppReflPrivate::MaybeCreateStaticType<unsigned int>("unsigned int"); }
	template <> inline const TypeInfo& StaticType<int>()				{ return CppReflPrivate::MaybeCreateStaticType<int>("int"); }
	template <> inline const TypeInfo& StaticType<unsigned long>()		{ return CppReflPrivate::MaybeCreateStaticType<unsigned long>("unsigned long"); }
	template <> inline const TypeInfo& StaticType<long>()				{ return CppReflPrivate::MaybeCreateStaticType<long>("long"); }
	template <> inline const TypeInfo& StaticType<unsigned long long>() { return CppReflPrivate::MaybeCreateStaticType<unsigned long long>("unsigned long long"); }
	template <> inline const TypeInfo& StaticType<long long>()			{ return CppReflPrivate::MaybeCreateStaticType<long long>("long long"); }
	template <> inline const TypeInfo& StaticType<float>()				{ return CppReflPrivate::MaybeCreateStaticType<float>("float"); }
	template <> inline const TypeInfo& StaticType<double>()				{ return CppReflPrivate::MaybeCreateStaticType<double>("double"); }
	template <> inline const TypeInfo& StaticType<long double>()		{ return CppReflPrivate::MaybeCreateStaticType<long double>("long double"); }
	template <> inline const TypeInfo& StaticType<void>()				{ return CppReflPrivate::MaybeCreateStaticType<void>("void"); }

	// Returns a static enum known at compile time.
	template <typename T>
	const EnumInfo& StaticEnum();

	// Returns a static class known at compile time.
	template <typename T>
	const ClassInfo& StaticClass();

	// Returns a static function known at compile time.
	template <void* FunctionAddress>
	const FunctionInfo& StaticFunction();


	// Returns the name of a reflected class.
	template <typename T>
	const char* GetTypeName()
	{
		return StaticType<T>().mName;
	}

	template <typename T>
	bool IsSameType(const TypeInfo& type)
	{
		return &type == &StaticType<T>();
	}

#if CPPREFL_CONCEPTS()
	template <typename T>
	concept ReflectedType = requires(T v)
	{
		{ cpprefl::StaticType<T>() } -> std::convertible_to<const cpprefl::TypeInfo&>;
	};

	template <typename T>
	concept ReflectedClass = requires(T v)
	{
		{ cpprefl::StaticClass<T>() } -> std::convertible_to<const cpprefl::ClassInfo&>;
	};

	template <typename T>
	concept ReflectedEnum = requires(T v)
	{
		{ cpprefl::StaticEnum<T>() } -> std::convertible_to<const cpprefl::EnumInfo&>;
	};
#endif
}
