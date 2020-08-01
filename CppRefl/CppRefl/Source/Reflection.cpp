#include "Reflection.h"

namespace refl
{
	Class Class::INVALID;
	Enum Enum::INVALID;


	///////////////////////////////////////////////////////////
	// Helper Function Prototypes
	///////////////////////////////////////////////////////////

	static std::string BuildIndentString(int indent);
	static std::string TypeToString(Type type);


	///////////////////////////////////////////////////////////
	// Element
	///////////////////////////////////////////////////////////

	std::string Element::ToString(int indent)const
	{
		return BuildIndentString(indent) + "mName";
	}


	///////////////////////////////////////////////////////////
	// Field
	///////////////////////////////////////////////////////////

	void* Field::GetRawDataPtr(void* blob)const
	{
		char* cBlob = static_cast<char*>(blob);
		return cBlob + mOffset;
	}

	std::string* Field::GetString(void* blob)const
	{
		if (mIsString) {
			return GetDataPtr<std::string>(blob);
		}

		return nullptr;
	}

	std::string Field::ToString(int indent)const
	{
		std::string s = BuildIndentString(indent);

		if (IsClassType()) {
			s += mClassType;
		}
		else if (IsEnumType()) {
			s += mEnumType;
		}
		else {
			s += TypeToString(mType);
		}

		return s + " " + mName;
	}

	///////////////////////////////////////////////////////////
	// Class
	///////////////////////////////////////////////////////////

	size_t Class::GetSize()const
	{
		size_t size = 0;
		for (auto field : mFields) {
			size += field.mSize;
		}

		return size;
	}

	std::string Class::ToString(int indent)const
	{
		std::string s = "";

		s += BuildIndentString(indent) + mName + " {";

		for (auto field : mFields) {
			s += "\n" + field.ToString(indent + 1);
		}

		for (auto function : mFunctions) {
			s += "\n" + function.ToString(indent + 1);
		}

		s += "\n" + BuildIndentString(indent) + "}";

		return s;
	}


	///////////////////////////////////////////////////////////
	// EnumValue
	///////////////////////////////////////////////////////////

	std::string EnumValue::ToString(int indent)const
	{
		return BuildIndentString(indent) + mName + " = " + std::to_string(mValue);
	}


	///////////////////////////////////////////////////////////
	// Enum
	///////////////////////////////////////////////////////////

	std::string Enum::GetValueString(int enumValue, bool qualified)const
	{
		if (mValueTable.find(enumValue) != mValueTable.end())
		{
			EnumValue value = mValueTable.at(enumValue);
			return (qualified) ? value.mQualifiedName : value.mName;
		}

		return "";
	}

	std::string Enum::ToString(int indent)const
	{
		std::string s = "";

		s += BuildIndentString(indent) + mName + " {";

		for (auto item : mValueTable) {
			s += "\n" + item.second.ToString(indent + 1);
		}

		s += "\n" + BuildIndentString(indent) + "}";

		return s;
	}


	///////////////////////////////////////////////////////////
	// ReflRegistry
	///////////////////////////////////////////////////////////

	const Class& Registry::GetClass(const std::string& className)const
	{
		if (mClasses.find(className) == mClasses.end()) {
			return Class::INVALID;
		}

		return mClasses.at(className);
	}

	const Enum& Registry::GetEnum(const std::string& enumName)const
	{
		if (mEnums.find(enumName) == mEnums.end()) {
			return Enum::INVALID;
		}

		return mEnums.at(enumName);
	}

	bool Registry::HasClass(const std::string& className)const
	{
		return mClasses.find(className) != mClasses.end();
	}

	bool Registry::HasEnum(const std::string& enumName)const
	{
		return mEnums.find(enumName) != mEnums.end();
	}

	bool Registry::ReflectClass(Class Class)
	{
		if (mClasses.find(Class.mQualifiedName) == mClasses.end()) {
			mClasses[Class.mQualifiedName] = Class;
			return true;
		}

		return false;
	}

	bool Registry::ReflectEnum(Enum Enum)
	{
		if (mEnums.find(Enum.mQualifiedName) == mEnums.end()) {
			mEnums[Enum.mQualifiedName] = Enum;
			return true;
		}

		return false;
	}





	static std::string BuildIndentString(int indent)
	{
		std::string s = "";
		for (int i = 0; i < indent; ++i) {
			s += "\t";
		}

		return s;
	}

	static std::string TypeToString(Type type)
	{
		static std::map<Type, std::string> Map = {
			{ Type::BOOL,			"bool" },
			{ Type::UINT8,			"uint8" },
			{ Type::INT8,			"int8" },
			{ Type::UINT16,			"uint16" },
			{ Type::INT16,			"int16" },
			{ Type::UINT32,			"uint32" },
			{ Type::INT32,			"int32" },
			{ Type::UINT64,			"uint64" },
			{ Type::INT64,			"int64" },
			{ Type::FLOAT,			"float" },
			{ Type::DOUBLE,			"double" },
			{ Type::LONG_DOUBLE,	"long double" }
		};

		if (Map.find(type) != Map.end()) {
			return Map[type];
		}

		return "";
	}
}