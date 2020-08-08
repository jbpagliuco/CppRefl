#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

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

		virtual bool operator==(const Element& rhs)const;
		virtual bool operator!=(const Element& rhs)const;

		bool HasAttribute(const std::string& attributeName);
		std::string GetAttribute(const std::string& attributeName);

		// Creates a string representation of this element.
		virtual std::string ToString(int indent = 0)const;

	protected:
		// A human-readable string containing all of our attributes.
		std::string GetAttrString()const;

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
		// Returns a pointer to the start of this field in a buffer of data.
		void* GetRawDataPtr(void* obj)const;

		// Returns a strongly-typed pointer to the start of this field in a buffer of data.
		template <typename T>
		T* GetDataPtr(void* obj)const;

		// Returns a string pointer to the start of this field in a buffer of data.
		std::string* GetString(void* obj)const;

		// Creates a string representation of this field.
		virtual std::string ToString(int indent = 0)const override;

		// Is this field a class type?
		bool IsClassType()const { return mClassType != ""; }
		// Is this field an enum type?
		bool IsEnumType()const { return mEnumType != ""; }
		// Is this field a primitive type?
		bool IsPrimitive()const { return !IsClassType() && !IsEnumType(); };

		// Is this field a string?
		bool IsString()const { return mIsString; }
		// Is this field an array?
		bool IsArray()const { return mIsArray; }

		// Is this field a pointer?
		bool IsPointer()const { return mIsPointer; }
		// Is this field const?
		bool IsConst()const { return mIsConst; }

	public:
		// Invaild reference to a Field.
		static Field INVALID;

	public:
		// Field data type.
		Type mType;

		// Size of this field.
		size_t mSize;
		// Offset into the containing class.
		size_t mOffset;

		// Underlying class type, if this field is a nested class.
		std::string mClassType;
		// Underlying enum type, if this field is an enum.
		std::string mEnumType;

		struct {
			// Is this field a string?
			bool mIsString : 1;
			// Is this field an array?
			bool mIsArray : 1;

			// Is thie field a pointer?
			bool mIsPointer : 1;
			// Is this field const?
			bool mIsConst : 1;
		};
	};

	// Represents a reflected function.
	class Function : public Element
	{
	public:
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Global functions

		// Invoke a reflected global function.
		template <typename... ParamTypes>
		void Invoke(ParamTypes&&... params)const;

		// Invoke a reflected global function that returns a value.
		template <typename ReturnType, typename... ParamTypes>
		std::optional<ReturnType> Invoke(ParamTypes&&... params)const;

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Member functions

		// Invoke a reflected member function.
		template <typename... ParamTypes>
		void Invoke(void* obj, ParamTypes&&... params)const;

		// Invoke a reflected member function that returns a value.
		template <typename ReturnType, typename... ParamTypes>
		std::optional<ReturnType> Invoke(void* obj, ParamTypes&&... params)const;

		// Invoke a reflected member function.
		template <typename ObjectType, typename... ParamTypes>
		void Invoke(ObjectType& obj, ParamTypes&&... params)const;

		// Invoke a reflected member function that returns a value.
		template <typename ReturnType, typename ObjectType, typename... ParamTypes>
		std::optional<ReturnType> Invoke(ObjectType& obj, ParamTypes&&... params)const;

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		// Creates a string representation of this class.
		virtual std::string ToString(int indent = 0)const override;

	private:
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// These two functions actually invoke the real function.
		void InvokeInternal(void* obj_or_null, void* param1_or_null = nullptr)const;

		template <typename ReturnType>
		std::optional<ReturnType> InvokeInternal(void* obj_or_null, void* param1_or_null = nullptr)const;
		//////////////////////////////////////////////////////////////////////////////////////////////////////

	public:
		// Invaild reference to a Function.
		static Function INVALID;

	public:
		// Return value type of this function (only primitive data types are supported).
		Type mReturnType = Type::VOID;

		// Function pointer
		FunctionType mFunction = nullptr;
		void* mFunctionInvoker = nullptr;
	};

	// Represents a reflected class or struct.
	class Class : public Element
	{
	public:
		// Get a field by name.
		const Field& GetField(const std::string& fieldName)const;
		Field& GetField(const std::string& fieldName);

		// Get a function by name.
		const Function& GetFunction(const std::string& functionName)const;
		Function& GetFunction(const std::string& functionName);

		// Creates a string representation of this class.
		virtual std::string ToString(int indent = 0)const override;

	public:
		// Invaild reference to a Class.
		static Class INVALID;

	public:
		// List of all fields in this class.
		std::vector<Field> mFields;
		// List of all functions in this class.
		std::vector<Function> mFunctions;
	};

	// Represents an individual enum value in a reflected enum type.
	class EnumValue : public Element
	{
	public:
		// Creates a string representation of this enum value.
		virtual std::string ToString(int indent = 0)const override;

	public:
		// The integer value of this enum value.
		int mValue;
	};

	// Represents a reflected enum type.
	class Enum : public Element
	{
	public:
		// Returns the name of an enum value.
		std::string GetValueString(int enumValue, bool qualified = false)const;

		// Creates a string representation of this enum.
		virtual std::string ToString(int indent = 0)const override;

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

	private:
		// Resolve function pointers for all Function's.
		void ResolveFunctions();

	private:
		// List of classes defined in this registry.
		std::map<std::string, Class> mClasses;
		// List of enums defined in this registry.
		std::map<std::string, Enum> mEnums;
		// List of global functions defined in this registry.
		std::map<std::string, Function> mFunctions;
	};
}

#include "Reflection.inl"