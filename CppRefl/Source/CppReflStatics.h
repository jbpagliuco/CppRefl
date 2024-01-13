#pragma once

#include <cassert>
#include <type_traits>

#include "Reflection/TypeInfo.h"

#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
#define CPPREFL_CONCEPTS() 1
#else
#define CPPREFL_CONCEPTS() 0
#endif

namespace CppReflPrivate
{
	// Helper to figure out if a template parameter has a member variable named "GetReflectedType".
	template <typename T, typename = int>
	struct HasGetReflectedType : std::false_type { };

	// Helper to figure out if a template parameter has a member variable named "GetReflectedType".
	template <typename T>
	struct HasGetReflectedType<T, decltype((void)T::StaticType, 0)> : std::true_type { };

	// Creates a static type if it doesn't already exist.
	template <typename T>
	const cpprefl::TypeInfo& MaybeCreateReflectedType(const char* typeName)
	{
		if constexpr (std::is_class_v<T> && HasGetReflectedType<T>::value)
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
	const TypeInfo& GetReflectedType();

	// Returns a static type known at compile-time.
	template <typename T>
	const TypeInfo& GetReflectedType()
	{
		if constexpr (!std::is_class_v<T> && !std::is_enum_v<T>)
		{
			const TypeInfo& type = CppReflPrivate::MaybeCreateReflectedType<T>("");
			assert(type.mName[0] != 0 && "Type info was never created. This is illegal.");

			return type;
		}

		static_assert(!std::is_class_v<T>, "This class doesn't implement GetReflectedType<>()");
		static_assert(!std::is_enum_v<T>, "This enum doesn't implement StaticEnum<>()");
	}

	// Static types for primitive types.
	template <> inline const TypeInfo& GetReflectedType<bool>()					{ return CppReflPrivate::MaybeCreateReflectedType<bool>("bool"); }
	template <> inline const TypeInfo& GetReflectedType<unsigned char>()		{ return CppReflPrivate::MaybeCreateReflectedType<unsigned char>("unsigned char"); }
	template <> inline const TypeInfo& GetReflectedType<char>()					{ return CppReflPrivate::MaybeCreateReflectedType<char>("char"); }
	template <> inline const TypeInfo& GetReflectedType<unsigned short>()		{ return CppReflPrivate::MaybeCreateReflectedType<unsigned short>("unsigned short"); }
	template <> inline const TypeInfo& GetReflectedType<short>()				{ return CppReflPrivate::MaybeCreateReflectedType<short>("short"); }
	template <> inline const TypeInfo& GetReflectedType<unsigned int>()			{ return CppReflPrivate::MaybeCreateReflectedType<unsigned int>("unsigned int"); }
	template <> inline const TypeInfo& GetReflectedType<int>()					{ return CppReflPrivate::MaybeCreateReflectedType<int>("int"); }
	template <> inline const TypeInfo& GetReflectedType<unsigned long>()		{ return CppReflPrivate::MaybeCreateReflectedType<unsigned long>("unsigned long"); }
	template <> inline const TypeInfo& GetReflectedType<long>()					{ return CppReflPrivate::MaybeCreateReflectedType<long>("long"); }
	template <> inline const TypeInfo& GetReflectedType<unsigned long long>()	{ return CppReflPrivate::MaybeCreateReflectedType<unsigned long long>("unsigned long long"); }
	template <> inline const TypeInfo& GetReflectedType<long long>()			{ return CppReflPrivate::MaybeCreateReflectedType<long long>("long long"); }
	template <> inline const TypeInfo& GetReflectedType<float>()				{ return CppReflPrivate::MaybeCreateReflectedType<float>("float"); }
	template <> inline const TypeInfo& GetReflectedType<double>()				{ return CppReflPrivate::MaybeCreateReflectedType<double>("double"); }
	template <> inline const TypeInfo& GetReflectedType<long double>()			{ return CppReflPrivate::MaybeCreateReflectedType<long double>("long double"); }
	template <> inline const TypeInfo& GetReflectedType<void>()					{ return CppReflPrivate::MaybeCreateReflectedType<void>("void"); }

	// Returns a static enum known at compile time.
	template <typename T>
	const EnumInfo& GetReflectedEnum();

	// Returns a static class known at compile time.
	template <typename T>
	const ClassInfo& GetReflectedClass();

	// Returns a static function known at compile time.
	template <void* FunctionAddress>
	const FunctionInfo& StaticFunction();


	// Returns the name of a reflected class.
	template <typename T>
	const char* GetTypeName()
	{
		return GetReflectedType<T>().mName;
	}

	template <typename T>
	bool IsSameType(const TypeInfo& type)
	{
		return &type == &GetReflectedType<T>();
	}

#if CPPREFL_CONCEPTS()
	template <typename T>
	concept ReflectedType = requires(T v)
	{
		{ cpprefl::GetReflectedType<T>() } -> std::convertible_to<const cpprefl::TypeInfo&>;
	};

	template <typename T>
	concept ReflectedClass = requires(T v)
	{
		{ cpprefl::GetReflectedClass<T>() } -> std::convertible_to<const cpprefl::ClassInfo&>;
	};

	template <typename T>
	concept ReflectedEnum = requires(T v)
	{
		{ cpprefl::GetReflectedEnum<T>() } -> std::convertible_to<const cpprefl::EnumInfo&>;
	};
#endif
}
