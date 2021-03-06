#pragma once

#include "ReflectionDefs.h"

#include <map>
#include <string>
#include <vector>

#include <optional>

#include "ReflectionBindings.h"
#include "ReflectionFunctionInvocation.h"
#include "ReflectionFunctionRegistration.h"
#include "ReflectionMarkup.h"
#include "ReflectionTypes.h"

namespace refl
{
	////////////////////////////////////////////
	// Classes
	////////////////////////////////////////////

	// Base class for all reflected data.
	class Element
	{
	public:
		Element();

		// Shallow check for equality/inequality.
		virtual bool operator==(const Element& rhs)const;
		virtual bool operator!=(const Element& rhs)const;

		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Element& rhs)const;

		bool HasAttribute(const std::string& attributeName)const;
		std::string GetAttribute(const std::string& attributeName)const;

#if defined(REFL_DEBUG)
		// Creates a string representation of this element.
		virtual std::string ToString(int indent = 0)const;

	protected:
		// A human-readable string containing all of our attributes.
		std::string GetAttrString()const;
#endif

	public:
		// Reflected name of this element.
		std::string mName;

		// Fully qualified name of this element (includes namespaces and classes).
		std::string mQualifiedName;

		// Reflected attributes of this element.
		std::map<std::string, std::string> mAttributes;

		// Pointer to the registry that contains this element.
		class Registry* mRegistry;
	};

	// Represents a reflected field in a reflected class.
	class Field : public Element
	{
	public:
		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Field& rhs)const;

		// Returns a pointer to the start of this field in a buffer of data.
		void* GetRawDataPtr(void* obj)const;

		// Returns a strongly-typed pointer to the start of this field in a buffer of data.
		template <typename T>
		T* GetDataPtr(void* obj)const;

		// Returns a string pointer to the start of this field in a buffer of data.
		// NB: This is equivalent to `char* str = GetDataPtr<char>(obj);`
		char* GetString(void* obj)const;

		template <typename T>
		T* GetArrayElement(void* obj, int index)const;

#if defined(REFL_DEBUG)
		// Creates a string representation of this field.
		virtual std::string ToString(int indent = 0)const override;
#endif

		// Is this field a class type?
		bool IsClassType()const { return mTypeInfo.mClassType != ""; }
		// Is this field an enum type?
		bool IsEnumType()const { return mTypeInfo.mEnumType != ""; }
		// Is this field a primitive type?
		bool IsPrimitive()const { return !IsClassType() && !IsEnumType(); };

		// Returns this field's class type, if it is a class.
		const class Class& GetClass()const;
		// Returns this field's enum type, if it is a class.
		const class Enum& GetEnum()const;

		// Is this field const?
		bool IsConst()const { return mTypeInfo.mIsConst; }
		// Is this field a pointer?
		bool IsPointer()const { return mTypeInfo.mIsPointer; }

		// Is this field a fixed size array?
		bool IsFixedArray()const { return mTypeInfo.IsFixedArray(); }
		// Is this field a dynamic array? (i.e. std::vector)
		bool IsDynamicArray()const { return mTypeInfo.IsDynamicArray(); }

		// Is this field a fixed size string?
		bool IsFixedString()const { return mTypeInfo.IsFixedString(); }
		// Is this field a dynamic string? (i.e. std::string)
		bool IsDynamicString()const { return mTypeInfo.IsDynamicString(); }

		// Is this field templated?
		bool IsTemplated()const { return mTypeInfo.IsTemplated(); }

	public:
		// Invaild reference to a Field.
		static Field INVALID;

	public:
		// Field type.
		TypeInfo mTypeInfo;

		// Offset into the containing class.
		// NB: This is the offset as defined by Clang. Sizes from other compilers may differ.
		size_t mOffset;
	};

	// Represents a reflected function.
	class Function : public Element
	{
	public:
		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Function& rhs)const;

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Global functions

		// Invoke a reflected global function.
		template <typename... ParamTypes>
		void InvokeGlobal(ParamTypes&&... params)const;

		// Invoke a reflected global function that returns a value.
		// NB: If you invoke this function, you must explicitly specify all parameter template arguments.
		template <typename ReturnType, typename... ParamTypes>
		std::optional<ReturnType> InvokeGlobal(ParamTypes&&... params)const;

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Member functions

		// Invoke a reflected member function.
		template <typename... ParamTypes>
		void InvokeMember(void* obj, ParamTypes&&... params)const;

		// Invoke a reflected member function that returns a value.
		template <typename ReturnType, typename... ParamTypes>
		std::optional<ReturnType> InvokeMember(void* obj, ParamTypes&&... params)const;

		// Invoke a reflected member function.
		template <typename ObjectType, typename... ParamTypes>
		void InvokeMember(ObjectType& obj, ParamTypes&&... params)const;

		// Invoke a reflected member function that returns a value.
		template <typename ReturnType, typename ObjectType, typename... ParamTypes>
		std::optional<ReturnType> InvokeMember(ObjectType& obj, ParamTypes&&... params)const;

		//////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(REFL_DEBUG)
		// Creates a string representation of this class.
		virtual std::string ToString(int indent = 0)const override;
#endif

	private:
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// These two functions actually invoke the real function.
		void InvokeInternal(void* obj_or_null, void* param1_or_null = nullptr)const;

		template <typename ReturnType>
		std::optional<ReturnType> InvokeInternal(void* obj_or_null, void* param1_or_null = nullptr)const;
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		// Check if we can safely invoke a function with the given parameters.
		bool ValidateInvocation(void* obj, void* param1)const;

	public:
		// Invaild reference to a Function.
		static Function INVALID;

	public:
		// Return value type of this function (only primitive data types are supported).
		DataType mReturnType = DataType::VOID;

		// Parameter type information.
		std::vector<TypeInfo> mParameterTypes;

		// Is this a member function or a global function?
		bool mIsMemberFunction;

		// Function pointer
		FunctionType mFunction = nullptr;
		void* mFunctionInvoker = nullptr;
	};

	// Represents a reflected class or struct.
	class Class : public Element
	{
	public:
		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Class& rhs)const;

		// Get a field by name.
		const Field& GetField(const std::string& fieldName)const;
		Field& GetField(const std::string& fieldName);

		// Get a function by name.
		const Function& GetFunction(const std::string& functionName)const;
		Function& GetFunction(const std::string& functionName);

#if defined(REFL_DEBUG)
		// Creates a string representation of this class.
		virtual std::string ToString(int indent = 0)const override;
#endif

	public:
		// Invaild reference to a Class.
		static Class INVALID;

	public:
		// List of all fields in this class.
		std::vector<Field> mFields;
		// List of all functions in this class.
		std::vector<Function> mFunctions;

		// This size of this class.
		// NB: This is the size as defined by Clang. Sizes from other compilers may differ.
		size_t mSize;
	};

	// Represents an individual enum value in a reflected enum type.
	class EnumValue : public Element
	{
	public:
		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const EnumValue& rhs)const;

#if defined(REFL_DEBUG)
		// Creates a string representation of this enum value.
		virtual std::string ToString(int indent = 0)const override;
#endif

	public:
		// The integer value of this enum value.
		int mValue;
	};

	// Represents a reflected enum type.
	class Enum : public Element
	{
	public:
		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Enum& rhs)const;

		// Returns the name of an enum value.
		std::string GetValueString(int enumValue, bool qualified = false)const;

#if defined(REFL_DEBUG)
		// Creates a string representation of this enum.
		virtual std::string ToString(int indent = 0)const override;
#endif

	public:
		// Invaild reference to a Enum.
		static Enum INVALID;

	public:
		// Table mapping integer values to enum values.
		std::map<int, EnumValue> mValueTable;
	};

	// Contains all reflected data for a translation unit.
	class Registry
	{
	public:
		// Finalizes the data in this registry (must be called after all reflected data is registered).
		bool Finalize();

		// Retrieves a reflected class representation from this registry, or Class::INVALID if it does not exist.
		const Class& GetClass(const std::string& className)const;
		// Retrieves a reflected enum representation from this registry, or Enum::INVALID if it does not exist.
		const Enum& GetEnum(const std::string& enumName)const;
		// Retrieves a reflected global function representation from this registry, or Function::INVALID if it does not exist.
		const Function& GetFunction(const std::string& functionName)const;

		// Does this registry contain the specified Class?
		bool HasClass(const std::string& className)const;
		// Does this registry contain the specified Enum?
		bool HasEnum(const std::string& enumName)const;
		// Does this registry contain the specified Function?
		bool HasFunction(const std::string& functionName)const;

		// Adds a reflected class to this registry.
		bool RegisterClass(Class reflClass);
		// Adds a reflected enum to this registry.
		bool RegisterEnum(Enum reflEnum);
		// Adds a reflected function to this registry.
		bool RegisterFunction(Function reflFunction);

		// Serialize this registry to a file.
		bool Export(const std::string& filename)const;
		// Import registry data from a file.
		bool Import(const std::string& filename);

	public:
		// Invaild reference to a Enum.
		static Registry INVALID;

	private:
		// Resolve function pointers for all Function's.
		void ResolveFunctions();

	public:
		// List of classes defined in this registry.
		std::map<std::string, Class> mClasses;
		// List of enums defined in this registry.
		std::map<std::string, Enum> mEnums;
		// List of global functions defined in this registry.
		std::map<std::string, Function> mFunctions;
	};
}

#include "Reflection.inl"