#include "Reflection.h"

#include "ErrorHandling.h"

namespace refl
{
	// Setup invalid references.
	Field Field::INVALID;
	Function Function::INVALID;
	Class Class::INVALID;
	Enum Enum::INVALID;
	Registry Registry::INVALID;

	// Serialization version number.
	constexpr int REFLECTION_VERSION_NUMBER = 1;

	///////////////////////////////////////////////////////////
	// Helper Function Prototypes
	///////////////////////////////////////////////////////////

	static std::string BuildIndentString(int indent);
	static std::string TypeToString(DataType type);

	///////////////////////////////////////////////////////////
	// Element
	///////////////////////////////////////////////////////////

	Element::Element() :
		mName(""),
		mQualifiedName(""),
		mRegistry(nullptr)
	{
	}

	bool Element::operator==(const Element& rhs)const
	{
		return mQualifiedName == rhs.mQualifiedName;
	}

	bool Element::operator!=(const Element& rhs)const
	{
		return mQualifiedName != rhs.mQualifiedName;
	}

	bool Element::DeepEquals(const Element& rhs)const
	{
		return mName == rhs.mName &&
			mQualifiedName == rhs.mQualifiedName &&
			mAttributes == rhs.mAttributes;
	}

	bool Element::HasAttribute(const std::string& attributeName)const
	{
		return mAttributes.find(attributeName) != mAttributes.end();
	}

	std::string Element::GetAttribute(const std::string& attributeName)const
	{
		if (HasAttribute(attributeName)) {
			return mAttributes.at(attributeName);
		}

		return "";
	}

#if defined(REFL_DEBUG)
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
#endif


	///////////////////////////////////////////////////////////
	// Field
	///////////////////////////////////////////////////////////

	bool Field::DeepEquals(const Field& rhs)const
	{
		if (!Element::DeepEquals(rhs)) {
			return false;
		}

		return mTypeInfo.DeepEquals(rhs.mTypeInfo) &&
			mOffset == rhs.mOffset;
	}

	void* Field::GetRawDataPtr(void* obj)const
	{
		char* cobj = static_cast<char*>(obj);
		return cobj + mOffset;
	}

	char* Field::GetString(void* obj)const
	{
		return GetDataPtr<char>(obj);
	}

	const Class& Field::GetClass()const
	{
		if (!IsClassType()) {
			REFL_INTERNAL_RAISE_ERROR("Tried to get class type of a non-class field [%s].", mQualifiedName);
			return Class::INVALID;
		}

		return mRegistry->GetClass(mTypeInfo.mClassType);
	}

	const Enum& Field::GetEnum()const
	{
		if (!IsEnumType()) {
			REFL_INTERNAL_RAISE_ERROR("Tried to get enum type of a non-enum field [%s].", mQualifiedName);
			return Enum::INVALID;
		}

		return mRegistry->GetEnum(mTypeInfo.mEnumType);
	}

#if defined(REFL_DEBUG)
	std::string Field::ToString(int indent)const
	{
		std::string s = BuildIndentString(indent);

		if (IsArray()) {
			s += TypeToString(mTypeInfo.mDataType) + " " + mName + "[" + std::to_string(mTypeInfo.mArraySize) + "] " + GetAttrString();
			return s;
		}
		if (IsClassType()) {
			s += mTypeInfo.mClassType;
		}
		else if (IsEnumType()) {
			s += mTypeInfo.mEnumType;
		}
		else {
			s += TypeToString(mTypeInfo.mDataType);
		}

		return s + " " + mName + " " + GetAttrString();
	}
#endif


	///////////////////////////////////////////////////////////
	// Function
	///////////////////////////////////////////////////////////

	bool Function::DeepEquals(const Function& rhs)const
	{
		if (!Element::DeepEquals(rhs)) {
			return false;
		}

		if (mParameterTypes.size() != rhs.mParameterTypes.size()) {
			return false;
		}
		for (int i = 0; i < mParameterTypes.size(); ++i) {
			if (!mParameterTypes[i].DeepEquals(rhs.mParameterTypes[i])) {
				return false;
			}
		}

		return mReturnType == rhs.mReturnType &&
			mIsMemberFunction == rhs.mIsMemberFunction;
	}

	// The real invoker.
	void Function::InvokeInternal(void* obj_or_null, void* param1_or_null)const
	{
		if (!ValidateInvocation(obj_or_null, param1_or_null)) {
			return;
		}

		mFunction(mFunctionInvoker, obj_or_null, nullptr, param1_or_null);
	}

	bool Function::ValidateInvocation(void* obj, void* param1)const
	{
		// Make sure the function is bound.
		if (mFunction == nullptr) {
			REFL_INTERNAL_RAISE_ERROR("Tried to invoke a function [%s] that was not bound.", mQualifiedName.c_str());
			return false;
		}

		// Make sure we have an object if this is a member function (and vice versa).
		if ((obj != nullptr) != mIsMemberFunction) {
			REFL_INTERNAL_RAISE_ERROR("Tried to invoke a %s function [%s] with %s.",
				mIsMemberFunction ? "member" : "global",
				mQualifiedName.c_str(),
				mIsMemberFunction ? "no object" : "an object");
			return false;
		}

		// Make sure the number of parameters match.
		const int numParams = (param1 == nullptr) ? 0 : 1;
		if (numParams != mParameterTypes.size()) {
			REFL_INTERNAL_RAISE_ERROR("Tried to invoke function [%s] with (%d) parameters, when (%d) are required.",
				mQualifiedName.c_str(),
				numParams,
				mParameterTypes.size());
			return false;
		}

		return true;
	}

#if defined(REFL_DEBUG)
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
#endif


	///////////////////////////////////////////////////////////
	// Class
	///////////////////////////////////////////////////////////

	bool Class::DeepEquals(const Class& rhs)const
	{
		if (!Element::DeepEquals(rhs)) {
			return false;
		}

		if (mFields.size() != rhs.mFields.size()) {
			return false;
		}
		for (int i = 0; i < mFields.size(); ++i) {
			if (!mFields[i].DeepEquals(rhs.mFields[i])) {
				return false;
			}
		}

		if (mFunctions.size() != rhs.mFunctions.size()) {
			return false;
		}
		for (int i = 0; i < mFunctions.size(); ++i) {
			if (!mFunctions[i].DeepEquals(rhs.mFunctions[i])) {
				return false;
			}
		}

		return mSize == rhs.mSize;
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

#if defined(REFL_DEBUG)
	std::string Class::ToString(int indent)const
	{
		std::string s = "";

		s += BuildIndentString(indent) + GetAttrString() + " " + mName + " {\n";

		s += BuildIndentString(indent + 1) + "Fields:";
		for (auto field : mFields) {
			s += "\n" + field.ToString(indent + 2);
		}

		s += "\n" + BuildIndentString(indent + 1) + "Functions:";
		for (auto function : mFunctions) {
			s += "\n" + function.ToString(indent + 2);
		}

		s += "\n" + BuildIndentString(indent) + "}";

		return s;
	}
#endif


	///////////////////////////////////////////////////////////
	// EnumValue
	///////////////////////////////////////////////////////////

	bool EnumValue::DeepEquals(const EnumValue& rhs)const
	{
		if (!Element::DeepEquals(rhs)) {
			return false;
		}

		return mValue == rhs.mValue;
	}

#if defined(REFL_DEBUG)
	std::string EnumValue::ToString(int indent)const
	{
		return BuildIndentString(indent) + mName + " = " + std::to_string(mValue) + " " + GetAttrString();
	}
#endif


	///////////////////////////////////////////////////////////
	// Enum
	///////////////////////////////////////////////////////////
	
	bool Enum::DeepEquals(const Enum& rhs)const
	{
		if (!Element::DeepEquals(rhs)) {
			return false;
		}

		if (mValueTable.size() != rhs.mValueTable.size()) {
			return false;
		}

		for (auto& value : mValueTable) {
			if (rhs.mValueTable.find(value.first) == rhs.mValueTable.end()) {
				return false;
			}

			if (value.second.DeepEquals(rhs.mValueTable.at(value.first))) {
				return false;
			}
		}

		return true;
	}

	std::string Enum::GetValueString(int enumValue, bool qualified)const
	{
		if (mValueTable.find(enumValue) != mValueTable.end())
		{
			EnumValue value = mValueTable.at(enumValue);
			return (qualified) ? value.mQualifiedName : value.mName;
		}

		REFL_INTERNAL_RAISE_ERROR("Failed to find enum value (%d) in [%s]", enumValue, mQualifiedName.c_str());
		return "";
	}

#if defined(REFL_DEBUG)
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
#endif


	///////////////////////////////////////////////////////////
	// ReflRegistry
	///////////////////////////////////////////////////////////

	bool Registry::Finalize()
	{
		// Resolve all function pointers.
		ResolveFunctions();

		// Add a reference to this registry in every element (there's probably a better a way to do this :))
		for (auto& reflClass : mClasses) {
			reflClass.second.mRegistry = this;

			for (auto& field : reflClass.second.mFields) {
				field.mRegistry = this;
			}
			for (auto& function : reflClass.second.mFunctions) {
				if (function.mFunction == nullptr) {
					REFL_INTERNAL_RAISE_ERROR("Member function [%s] was not bound. Use REFL_BIND_* functions in ReflectionBindings.h to bind it.", function.mQualifiedName.c_str());
				}

				function.mRegistry = this;
			}
		}

		for (auto& reflEnum : mEnums) {
			reflEnum.second.mRegistry = this;

			for (auto& value : reflEnum.second.mValueTable) {
				value.second.mRegistry = this;
			}
		}

		for (auto& reflFunction : mFunctions) {
			// Make sure this function is bound.
			if (reflFunction.second.mFunction == nullptr) {
				REFL_INTERNAL_RAISE_ERROR("Global function [%s] was not bound. Use REFL_BIND_* functions in ReflectionBindings.h to bind it.", reflFunction.second.mQualifiedName.c_str());
			}

			reflFunction.second.mRegistry = this;
		}

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

	const Function& Registry::GetFunction(const std::string& functionName)const
	{
		if (mFunctions.find(functionName) == mFunctions.end()) {
			return Function::INVALID;
		}

		return mFunctions.at(functionName);
	}

	bool Registry::HasClass(const std::string& className)const
	{
		return mClasses.find(className) != mClasses.end();
	}

	bool Registry::HasEnum(const std::string& enumName)const
	{
		return mEnums.find(enumName) != mEnums.end();
	}

	bool Registry::HasFunction(const std::string& functionName)const
	{
		return mFunctions.find(functionName) != mFunctions.end();
	}

	bool Registry::RegisterClass(Class reflClass)
	{
		if (mClasses.find(reflClass.mQualifiedName) == mClasses.end()) {
			mClasses[reflClass.mQualifiedName] = reflClass;
			return true;
		}

		return false;
	}

	bool Registry::RegisterEnum(Enum reflEnum)
	{
		if (mEnums.find(reflEnum.mQualifiedName) == mEnums.end()) {
			mEnums[reflEnum.mQualifiedName] = reflEnum;
			return true;
		}

		return false;
	}

	bool Registry::RegisterFunction(Function reflFunction)
	{
		if (mFunctions.find(reflFunction.mQualifiedName) == mFunctions.end()) {
			mFunctions[reflFunction.mQualifiedName] = reflFunction;
			return true;
		}

		return false;
	}

	void Registry::ResolveFunctions()
	{
		for (auto &registration : GetFunctionRegistrations()) {
			// Is this a global function?
			if (registration->mQualifiedClassName == "") {
				if (HasFunction(registration->mFunctionName)) {
					// Bind the real function to our reflected function representation.
					mFunctions[registration->mFunctionName].mFunction = registration->mFunction;
					mFunctions[registration->mFunctionName].mFunctionInvoker = registration->mFunctionInvoker;
				}
				else {
					REFL_INTERNAL_RAISE_ERROR("Failed to find global function [%s] while resolving functions.", registration->mFunctionName.c_str());
				}
				continue;
			}

			// This function is a member of a class.

			// Find this function's class in the registry
			Class& reflClass = mClasses[registration->mQualifiedClassName];
			if (reflClass == Class::INVALID) {
				REFL_INTERNAL_RAISE_ERROR("Failed to find class [%s] while resolving functions.", registration->mQualifiedClassName.c_str());
				continue;
			}
			
			// Find this function within the class
			Function& reflFunction = reflClass.GetFunction(registration->mFunctionName);
			if (reflFunction == Function::INVALID) {
				REFL_INTERNAL_RAISE_ERROR("Failed to find function definition [%s::%s] while resolving functions.", registration->mQualifiedClassName.c_str(), registration->mFunctionName.c_str());
				continue;
			}

			// Bind the real function to our reflected function representation.
			reflFunction.mFunction = registration->mFunction;
			reflFunction.mFunctionInvoker = registration->mFunctionInvoker;
		}
	}

	bool Registry::Export(const std::string& filename)const
	{
		return true;
	}

	bool Registry::Import(const std::string& filename)
	{
		return true;
	}




#if defined(REFL_DEBUG)
	static std::string BuildIndentString(int indent)
	{
		std::string s = "";
		for (int i = 0; i < indent; ++i) {
			s += "\t";
		}

		return s;
	}

	static std::string TypeToString(DataType type)
	{
		static std::map<DataType, std::string> Map = {
			{ DataType::BOOL,			"bool" },
			{ DataType::UINT8,			"uint8" },
			{ DataType::INT8,			"int8" },
			{ DataType::UINT16,			"uint16" },
			{ DataType::INT16,			"int16" },
			{ DataType::UINT32,			"uint32" },
			{ DataType::INT32,			"int32" },
			{ DataType::UINT64,			"uint64" },
			{ DataType::INT64,			"int64" },
			{ DataType::FLOAT,			"float" },
			{ DataType::DOUBLE,			"double" },
			{ DataType::LONG_DOUBLE,	"long double" },
			{ DataType::VOID,			"void" }
		};

		if (Map.find(type) != Map.end()) {
			return Map[type];
		}

		return "";
	}
#endif
}