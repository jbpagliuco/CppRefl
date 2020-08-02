#include "Reflection.h"

namespace refl
{
	Field Field::INVALID;
	Function Function::INVALID;
	Class Class::INVALID;
	Enum Enum::INVALID;


	///////////////////////////////////////////////////////////
	// Helper Function Prototypes
	///////////////////////////////////////////////////////////

	static std::vector<FunctionRegistration*>& GetFunctionRegistrations();

	static std::string BuildIndentString(int indent);
	static std::string TypeToString(Type type);


	///////////////////////////////////////////////////////////
	// Element
	///////////////////////////////////////////////////////////

	Element::Element() :
		mName(""),
		mQualifiedName("")
	{
	}

	bool Element::operator==(const Element& rhs)const
	{
		return mQualifiedName == rhs.mQualifiedName;
	}

	bool Element::HasAttribute(const std::string& attributeName)
	{
		return mAttributes.find(attributeName) != mAttributes.end();
	}

	std::string Element::GetAttribute(const std::string& attributeName)
	{
		if (HasAttribute(attributeName)) {
			return mAttributes[attributeName];
		}

		return "";
	}

	std::string Element::ToString(int indent)const
	{
		return BuildIndentString(indent) + mName;
	}

	std::string Element::GetAttrString()const
	{
		std::string output = "";

		for (auto it : mAttributes) {
			if (output != "") {
				output += " ";
			}
			output += "[" + it.first;
			
			if (it.second != "") {
				output += "=" + it.second;
			}

			output += "]";
		}

		return output;
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

		return s + " " + mName + " " + GetAttrString();
	}


	///////////////////////////////////////////////////////////
	// Function
	///////////////////////////////////////////////////////////

	void Function::Invoke(void* self)const
	{
		if (mFunction != nullptr) {
			mFunction(self, nullptr);
		}
	}

	std::string Function::ToString(int indent)const
	{
		std::string s = BuildIndentString(indent);

		s += TypeToString(mReturnType);

		const std::string attrs = GetAttrString();
		if (attrs != "") {
			s += " " + attrs;
		}
		s += " " + mName + "()";

		return s;
	}


	///////////////////////////////////////////////////////////
	// FunctionRegistration
	///////////////////////////////////////////////////////////

	static std::vector<FunctionRegistration*>& GetFunctionRegistrations()
	{
		// Must be wrapped in a function to make sure it's initialized before use.
		static std::vector<FunctionRegistration*> FunctionRegistrations;
		
		return FunctionRegistrations;
	}

	FunctionRegistration::FunctionRegistration(const std::string& qualifiedClassName, const std::string& functionName, Function::FunctionType function) :
		mQualifiedClassName(qualifiedClassName),
		mFunctionName(functionName),
		mFunction(function)
	{
		GetFunctionRegistrations().push_back(this);
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

	const Field& Class::GetField(const std::string& fieldName)const
	{
		for (const auto& field : mFields) {
			if (field.mName == fieldName) {
				return field;
			}
		}

		return Field::INVALID;
	}

	Field& Class::GetField(const std::string& fieldName)
	{
		for (auto& field : mFields) {
			if (field.mName == fieldName) {
				return field;
			}
		}

		return Field::INVALID;
	}

	const Function& Class::GetFunction(const std::string& functionName)const
	{
		for (const auto& function : mFunctions) {
			if (function.mName == functionName) {
				return function;
			}
		}

		return Function::INVALID;
	}

	Function& Class::GetFunction(const std::string& functionName)
	{
		for (auto& function : mFunctions) {
			if (function.mName == functionName) {
				return function;
			}
		}

		return Function::INVALID;
	}

	std::string Class::ToString(int indent)const
	{
		std::string s = "";

		s += BuildIndentString(indent) + GetAttrString() + " " + mName + " {";

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
		return BuildIndentString(indent) + mName + " = " + std::to_string(mValue) + " " + GetAttrString();
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

		s += BuildIndentString(indent) + GetAttrString() + " " + mName + " {";

		for (auto item : mValueTable) {
			s += "\n" + item.second.ToString(indent + 1);
		}

		s += "\n" + BuildIndentString(indent) + "}";

		return s;
	}


	///////////////////////////////////////////////////////////
	// ReflRegistry
	///////////////////////////////////////////////////////////

	bool Registry::Finalize()
	{
		ResolveFunctions();

		return true;
	}

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

	bool Registry::RegisterClass(Class Class)
	{
		if (mClasses.find(Class.mQualifiedName) == mClasses.end()) {
			mClasses[Class.mQualifiedName] = Class;
			return true;
		}

		return false;
	}

	bool Registry::RegisterEnum(Enum Enum)
	{
		if (mEnums.find(Enum.mQualifiedName) == mEnums.end()) {
			mEnums[Enum.mQualifiedName] = Enum;
			return true;
		}

		return false;
	}

	void Registry::ResolveFunctions()
	{
		for (auto &registration : GetFunctionRegistrations()) {
			// Find this function's class in the registry
			Class& reflClass = mClasses[registration->mQualifiedClassName];
			if (reflClass == Class::INVALID) {
				continue;
			}
			
			// Find this function within the class
			Function& reflFunction = reflClass.GetFunction(registration->mFunctionName);
			if (reflFunction == Function::INVALID) {
				continue;
			}

			reflFunction.mFunction = registration->mFunction;
		}
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
			{ Type::LONG_DOUBLE,	"long double" },
			{ Type::VOID,			"void" }
		};

		if (Map.find(type) != Map.end()) {
			return Map[type];
		}

		return "";
	}
}