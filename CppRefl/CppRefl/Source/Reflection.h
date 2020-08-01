#pragma once

#include <map>
#include <string>
#include <vector>

#include "ReflectionMarkup.h"

namespace refl
{
	////////////////////////////////////////////
	// Enums
	////////////////////////////////////////////

	// All support data types.
	enum class Type
	{
		INVALID = 0,

		BOOL = 1,

		UINT8,
		INT8,
		UINT16,
		INT16,
		UINT32,
		INT32,
		UINT64,
		INT64,

		FLOAT,
		DOUBLE,
		LONG_DOUBLE,

		CLASS, // struct, class, record, whatever
	};


	////////////////////////////////////////////
	// Classes
	////////////////////////////////////////////

	// Base class for all reflected data.
	class Element
	{
	public:
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
	};

	// Represents a reflected field in a reflected class.
	class Field : public Element
	{
	public:
		// Returns a pointer to the start of this field in blob of data.
		void* GetRawDataPtr(void* blob)const;

		// Returns a strongly-typed pointer to the start of this field in a blob of data.
		template <typename T>
		T* GetDataPtr(void* blob)const;

		// Returns a string pointer to the start of this field in a blob of data.
		std::string* GetString(void* blob)const;

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
	};

	// Represents a reflected class or struct.
	class Class : public Element
	{
	public:
		// Returns the size of this class.
		size_t GetSize()const;

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
		// Retrieves a reflected class representation from this registry, or Class::INVALID if it does not exist.
		const Class& GetClass(const std::string& className)const;
		// Retrieves a reflected enum representation from this registry, or Enum::INVALID if it does not exist.
		const Enum& GetEnum(const std::string& enumName)const;

		// Does this registry contain the specified Class?
		bool HasClass(const std::string& className)const;
		// Does this registry contain the specified Enum?
		bool HasEnum(const std::string& enumName)const;

		// Adds a reflected class to this registry.
		bool RegisterClass(Class Class);
		// Adds a reflected enum to this registry.
		bool RegisterEnum(Enum Enum);

	private:
		// List of classes defined in this registry.
		std::map<std::string, Class> mClasses;
		// List of enums defined in this registry.
		std::map<std::string, Enum> mEnums;
	};
}

#include "Reflection.inl"