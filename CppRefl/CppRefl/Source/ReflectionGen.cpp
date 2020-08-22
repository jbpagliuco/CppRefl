#include "ReflectionGen.h"

#include <clang-c/Index.h>

#include "Reflection.h"

namespace refl
{
	class RegistryGenerator
	{
	public:
		bool Generate(Registry& outRegistry, GenerationParameters params);

	private:
		bool ReportClangErrors(CXTranslationUnit tu);
		CXTranslationUnit CreateTranslationUnit(CXIndex index);

		bool BuildReflection(CXCursor cursor);

		bool ShouldReflectCursor(CXCursor cursor);
		bool BuildTypeInfo(TypeInfo& typeInfo, int &stdOffset, CXType cursorType);

		bool ReflectElement(Element& reflElement, CXCursor cursor);

		bool ReflectField(Field& reflField, int& stdFieldOffset, CXCursor cursor, CXCursor parent);
		bool ReflectFunction(Function& reflFunction, CXCursor cursor, bool isMemberFunction);

		bool ReflectClassMembers(Class& reflClass, int& stdFieldOffset, CXCursor classCursor, std::vector<CXCursor> memberCursors);
		Class ReflectClass(CXCursor cursor);

		bool ReflectEnumValue(EnumValue& reflEnumValue, CXCursor cursor);
		Enum ReflectEnum(CXCursor cursor);

	private:
		GenerationParameters mParams;
		Registry* mRegistry = nullptr;

		CXIndex mIndex;
		CXTranslationUnit mTU;
	};


	//////////////////////////////////////////////////////////////////////////////

	// Print the clang version to stdout
	static void PrintClangVersion()
	{
		auto str = clang_getClangVersion();
		auto str2 = clang_getCString(str);
		printf("Clang Version: %s\n\n", str2);
		clang_disposeString(str);
	}

	// Get a beautified name string for a cursor.
	static std::string GetCursorName(CXCursor cursor)
	{
		CXString cxString = clang_getCursorSpelling(cursor);
		std::string name(clang_getCString(cxString));
		clang_disposeString(cxString);

		return name;
	}

	// Get a fully qualified name string (including namespaces) for a cursor.
	// Strips the :: from global namespaces.
	static std::string GetFullyQualifiedCursorName(CXCursor cursor)
	{
		std::string name = GetCursorName(cursor);

		cursor = clang_getCursorSemanticParent(cursor);
		while (cursor.kind != CXCursor_FirstInvalid && cursor.kind != CXCursor_TranslationUnit)
		{
			const std::string curName = GetCursorName(cursor);
			if (curName != "") {
				name = curName + "::" + name;
			}

			cursor = clang_getCursorSemanticParent(cursor);
		}

		return name;
	}

	// Collects all top level children cursors from a parent cursor.
	static std::vector<CXCursor> CollectTopLevelChildren(CXCursor cursor)
	{
		auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData clientData)
		{
			std::vector<CXCursor>* children = static_cast<std::vector<CXCursor>*>(clientData);

			children->push_back(cursor);
			return CXChildVisit_Continue;
		};

		std::vector<CXCursor> children;
		clang_visitChildren(cursor, visitor, &children);

		return children;
	}

	// Collects annotations from a cursor.
	static std::vector<std::string> CollectCursorAnnotations(CXCursor cursor)
	{
		auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData clientData)
		{
			std::vector<std::string>* children = static_cast<std::vector<std::string>*>(clientData);

			// Collect all the annotations that contain our custom reflection markup
			if (cursor.kind == CXCursor_AnnotateAttr) {
				const std::string annotation = GetCursorName(cursor);
				if (annotation.find(REFL_INTERNAL_ANNOTATION) != std::string::npos) {
					children->push_back(annotation);
				}
			}
			return CXChildVisit_Continue;
		};

		std::vector<std::string> children;
		clang_visitChildren(cursor, visitor, &children);

		return children;
	}

	// Get the underlying data type from a CXType.
	// e.g. translate pointers to pointees, typedefs to underlying type, arrays to element types
	static CXType GetRealDataType(CXType type)
	{
		// Get pointee type for pointers
		const CXType pointeeType = clang_getPointeeType(type);
		if (pointeeType.kind != CXType_Invalid) {
			type = pointeeType;
		}

		// Get array element type for fixed size arrays
		const CXType arrayElementType = clang_getArrayElementType(type);
		if (arrayElementType.kind != CXType_Invalid) {
			type = arrayElementType;
		}

		// Do typedef translation
		if (type.kind == CXType_Typedef) {
			type = clang_getCanonicalType(type);
		}

		// See if this cursor can be translated into a different decl
		const CXCursor translatedCursor = clang_getTypeDeclaration(type);
		if (translatedCursor.kind != CXCursor_NoDeclFound) {
			// Do enum translation
			if (type.kind == CXType_Enum) {
				type = clang_getEnumDeclIntegerType(translatedCursor);
			}
		}

		return type;
	}

	// Maps clang cursor types to refl::DataType.
	static DataType GetDataTypeFromClang(CXType type)
	{
		static std::map<CXTypeKind, DataType> Map = {
			{ CXType_Bool,			DataType::BOOL },
			{ CXType_Char_U,		DataType::UINT8 },
			{ CXType_UChar,			DataType::UINT8 },
			{ CXType_Char16,		DataType::INT16 },
			{ CXType_Char32,		DataType::INT32 },
			{ CXType_UShort,		DataType::UINT16 },
			{ CXType_UInt,			DataType::UINT32 },
			{ CXType_ULong,			DataType::UINT32 },
			{ CXType_ULongLong,		DataType::UINT64 },
			{ CXType_Char_S,		DataType::INT8 },
			{ CXType_SChar,			DataType::INT8 },
			{ CXType_WChar,			DataType::INT16 },
			{ CXType_Short,			DataType::INT16},
			{ CXType_Int,			DataType::INT32 },
			{ CXType_Long,			DataType::INT32 },
			{ CXType_LongLong,		DataType::INT64 },
			{ CXType_Float,			DataType::FLOAT },
			{ CXType_Double,		DataType::DOUBLE },
			{ CXType_LongDouble,	DataType::LONG_DOUBLE },
			{ CXType_Void,			DataType::VOID },
			{ CXType_Record,		DataType::CLASS },
		};

		type = GetRealDataType(type);

		const CXCursor typeDeclaration = clang_getTypeDeclaration(type);

		// Unions would show up as a record type, so let's make extra sure that we're dealing with a union here.
		if (typeDeclaration.kind != CXCursor_NoDeclFound) {
			if (typeDeclaration.kind == CXCursor_UnionDecl) {
				return DataType::UNION;
			}
			else if (typeDeclaration.kind == CXCursor_StructDecl || typeDeclaration.kind == CXCursor_ClassDecl) {
				return DataType::CLASS;
			}
		}

		// Recognized type?
		if (Map.find(type.kind) != Map.end()) {
			return Map[type.kind];
		}

		return DataType::INVALID;
	}

	// Maps clang cursor types to Type's.
	static DataType GetDataTypeFromClang(CXCursor cursor)
	{
		return GetDataTypeFromClang(clang_getCursorType(cursor));
	}

	static bool IsSupportedStdType(const std::string& className)
	{
		return 
			className == "std::map" ||
			className == "std::string" ||
			className == "std::vector";
	}

	static size_t GetStdClassSize(CXCursor cursor, CompilerType target)
	{
		const std::string className = GetFullyQualifiedCursorName(cursor);
		if (!IsSupportedStdType(className)) {
			REFL_INTERNAL_RAISE_ERROR("Tried to reflect unsupported std class type [%s]", className.c_str());
			return -1;
		}
		
		typedef std::map<std::string, size_t> SizeMap;
		
		// Don't actually have to hardcode clang
		const SizeMap clangSizes = {
			{ className, clang_Type_getSizeOf(clang_getCursorType(cursor)) }
		};

		const SizeMap msvcSizes = {
			{ "std::map", 24 },
			{ "std::string", 40 },
			{ "std::vector", 32 },
		};

		const std::map<CompilerType, SizeMap> compilerSizes = {
			{ CompilerType::CLANG, clangSizes },
			{ CompilerType::MSVC, msvcSizes }
		};

		//////////////////////////////////////////////////////////////////

		if (compilerSizes.find(target) == compilerSizes.end()) {
			REFL_INTERNAL_RAISE_ERROR("Unknown compiler type (%d) found when determining size of class [%s]", (int)target, className.c_str());
			return -1;
		}

		if (compilerSizes.at(target).find(className) == compilerSizes.at(target).end()) {
			REFL_INTERNAL_RAISE_ERROR("Unhandled class type [%s] for compiler (%d)", className.c_str(), (int)target);
			return -1;
		}

		return compilerSizes.at(target).at(className);
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	
	bool RegistryGenerator::ReportClangErrors(CXTranslationUnit tu)
	{
		bool errors = false;

		CXDiagnosticSet diagnosticSet = clang_getDiagnosticSetFromTU(tu);
		const int numDiagnostics = clang_getNumDiagnosticsInSet(diagnosticSet);
		for (int i = 0; i < numDiagnostics; ++i) {
			CXDiagnostic diagnostic = clang_getDiagnosticInSet(diagnosticSet, i);

			CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);

			// Low-severity, skip
			if (severity < CXDiagnosticSeverity::CXDiagnostic_Warning) {
				continue;
			}
			
			// Get diagnostic information
			CXString diagnosticString = clang_getDiagnosticSpelling(diagnostic);

			CXFile file;
			unsigned int line;
			clang_getExpansionLocation(clang_getDiagnosticLocation(diagnostic), &file, &line, nullptr, nullptr);

			CXString cxFilePath = clang_getFileName(file);

			// Raise error, if necessary
			if (severity == CXDiagnosticSeverity::CXDiagnostic_Warning) {
				refl::RaiseErrorInternal(ErrorType::WARNING, clang_getCString(cxFilePath), line, clang_getCString(diagnosticString));
				errors |= mParams.mRaiseClangWarnings;
			}
			else {
				refl::RaiseErrorInternal(ErrorType::ERROR, clang_getCString(cxFilePath), line, clang_getCString(diagnosticString));
				errors |= mParams.mRaiseClangErrors;
			}
			
			// Cleanup
			clang_disposeString(cxFilePath);
			clang_disposeString(diagnosticString);
		}

		return errors;
	}

	CXTranslationUnit RegistryGenerator::CreateTranslationUnit(CXIndex index)
	{
		// See if there was a specified driver type
		std::string driverMode;
		for (const auto& arg : mParams.mClangArgs) {
			const std::string driverModeStr = "--driver-mode=";
			if (arg.find(driverModeStr) != std::string::npos) {
				driverMode = arg.substr(arg.find(driverModeStr) + driverModeStr.size());
				break;
			}
		}

		////////////////////////////////////////////////////////////////////////////

		std::map<std::string, std::string> INCLUDE_COMPILER_FLAGS = {
			{ "", "-I" }, // default clang
			{ "cl", "/I" }, // cl driver
		};

		std::map<std::string, std::string> DEFINE_COMPILER_FLAGS = {
			{ "", "-D" }, // default clang
			{ "cl", "/D" }, // cl driver
		};

		if (INCLUDE_COMPILER_FLAGS.find(driverMode) == INCLUDE_COMPILER_FLAGS.end()) {
			REFL_INTERNAL_RAISE_ERROR("Unrecognized driver type: '%s'", driverMode.c_str());
			return nullptr;
		}

		////////////////////////////////////////////////////////////////////////////

		// Temp buffer of std::string args
		std::vector<std::string> stringArgs;

		// Add include paths
		for (const auto& path : mParams.mIncludePaths) {
			stringArgs.push_back(INCLUDE_COMPILER_FLAGS[driverMode] + path);
		}

		// Add cpp refl flags
		stringArgs.push_back(DEFINE_COMPILER_FLAGS[driverMode] + "CPP_REFL_BUILD_REFLECTION"); // Creates a CPP_REFL_BUILD_REFLECTION macro

		////////////////////////////////////////////////////////////////////////////

		// Final list of raw args
		std::vector<const char*> allArgs;

		// Add user-given clang args
		for (const auto& arg : mParams.mClangArgs) {
			allArgs.push_back(const_cast<char*>(arg.c_str()));
		}

		// Add our cpp refl args
		for (const auto& arg : stringArgs) {
			allArgs.push_back(const_cast<char*>(arg.c_str()));
		}

		return clang_createTranslationUnitFromSourceFile(index, mParams.mInputFilepath.c_str(), (int)allArgs.size(), &allArgs[0], 0, nullptr);
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	bool RegistryGenerator::ShouldReflectCursor(CXCursor cursor)
	{
		CXSourceLocation sourceLocation = clang_getCursorLocation(cursor);

		CXFile file;
		clang_getExpansionLocation(sourceLocation, &file, nullptr, nullptr, nullptr);
		if (file == nullptr) {
			return false;
		}

		CXString cxFilePath = clang_getFileName(file);
		std::string filepath(clang_getCString(cxFilePath));
		clang_disposeString(cxFilePath);

		// exclude anything outside the project
		if (mParams.mProjectPath != "" && filepath.rfind(mParams.mProjectPath, 0) != 0) {
			return false;
		}

		// always recurse into namespaces
		if (cursor.kind == CXCursor_Namespace) {
			return true;
		}

		// include anything that is explicitly reflected
		if (CollectCursorAnnotations(cursor).size() > 0) {
			return true;
		}

		return false;
	}
	
	bool RegistryGenerator::BuildTypeInfo(TypeInfo& typeInfo, int& stdOffset, CXType cursorType)
	{
		const CXType realDataType = GetRealDataType(cursorType);

		// Type/Size
		typeInfo.mDataType = GetDataTypeFromClang(realDataType);
		typeInfo.mSize = clang_Type_getSizeOf(realDataType);

		// Const
		typeInfo.mIsConst = clang_isConstQualifiedType(cursorType);

		// Pointers
		typeInfo.mIsPointer = cursorType.kind == CXType_Pointer;
		if (typeInfo.mIsPointer) {
			// We want the size of the pointee
			typeInfo.mSize = clang_Type_getSizeOf(clang_getPointeeType(cursorType));
		}

		// Fixed size arrays
		if (cursorType.kind == CXType_ConstantArray) {
			typeInfo.mArraySize = clang_getArraySize(cursorType);
		}

		// Complex data types
		const CXCursor typeDeclaration = clang_getTypeDeclaration(cursorType);
		const std::string typeDeclarationQName = GetFullyQualifiedCursorName(typeDeclaration);
		if (typeInfo.mDataType == DataType::CLASS)
		{
			typeInfo.mClassType = typeDeclarationQName;
		}
		else if (cursorType.kind == CXType_Enum)
		{
			typeInfo.mEnumType = typeDeclarationQName;
		}

		// Raise warnings when using std types, as the internal layouts of each class can differ between compilers.
		if (typeDeclarationQName.rfind("std::", 0) == 0) {
			if (!mParams.mDisableStdWarnings) {
				REFL_INTERNAL_RAISE_WARNING(
					"Tried to reflect a class in the std namespace [%s]. This is discouraged, as the implementations of these classes can differ between compilers.",
					typeDeclarationQName.c_str());
			}

			// Nevertheless, try to support std classes when possible.

			// Special container types
			bool handled = false;
			if (typeDeclarationQName == "std::string") {
				handled = true;
				typeInfo.IsDynamic = true;
			}
			else {
				handled = true;
				typeInfo.IsDynamic = true;
			}

			if (handled) {
				// Fill out some common fields
				const size_t clangSize = GetStdClassSize(typeDeclaration, CompilerType::CLANG);
				const size_t realSize = GetStdClassSize(typeDeclaration, mParams.mTargetCompiler);
				stdOffset = (int)realSize - (int)clangSize;
				typeInfo.mSize = realSize;

				typeInfo.mDataType = DataType::CLASS;
				typeInfo.mClassType = typeDeclarationQName;
			}
			else {
				// Always raise warnings about these. Either they must be supported, or some other non-std class must be used.
				REFL_INTERNAL_RAISE_ERROR("Unrecognized class type from the std namespace [%s].", typeDeclarationQName.c_str());
				return false;
			}
		}

		// Gather all templated types
		for (int i = 0; i < clang_Type_getNumTemplateArguments(cursorType); ++i) {
			const CXType templateType = clang_Type_getTemplateArgumentAsType(cursorType, i);
			const CXCursor templateCursor = clang_getTypeDeclaration(templateType);

			// Don't reflect built-in std templated types.
			const std::string name = GetFullyQualifiedCursorName(templateCursor);
			if (name.rfind("std::") == 0 && !IsSupportedStdType(name)) {
				continue;
			}
			
			TypeInfo templateTypeInfo;
			int temp;
			if (BuildTypeInfo(templateTypeInfo, temp, templateType)) {
				typeInfo.mTemplateTypes.push_back(templateTypeInfo);
			}
			else {
				return false;
			}
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	// Gathers common reflection information for any refl::Element type.
	bool RegistryGenerator::ReflectElement(Element& reflElement, CXCursor cursor)
	{
		// Names
		reflElement.mName = GetCursorName(cursor);
		reflElement.mQualifiedName = GetFullyQualifiedCursorName(cursor);

		// Attributes
		for (auto annotation : CollectCursorAnnotations(cursor)) {
			// Attributes are split by a special separator. e.g.:
			// annotate(cpp_refl)
			// annotate(cpp_refl,just_a_tag)
			// annotate(cpp_ref,tag_with_a_value,the_value)
			const size_t firstSeparator = annotation.find(REFL_INTERNAL_ANNOTATION_SEPARATOR);

			// Is this actually an attribute?
			if (firstSeparator == std::string::npos) {
				continue;
			}

			const std::string metadata = annotation.substr(firstSeparator + 1);

			// If this separator doesn't exist, this is just a tag with no value
			const size_t secondSeparator = metadata.find(REFL_INTERNAL_ANNOTATION_SEPARATOR);
			if (secondSeparator == std::string::npos) {
				reflElement.mAttributes[metadata] = "";
			}
			else {
				// This is a tag+value combo
				const std::string tag = metadata.substr(0, secondSeparator);
				const std::string value = metadata.substr(secondSeparator + 1);
				reflElement.mAttributes[tag] = value;
			}
		}

		return true;
	}

	// Reflects a field within a record.
	bool RegistryGenerator::ReflectField(Field& reflField, int& stdFieldOffset, CXCursor cursor, CXCursor parent)
	{
		// Reflect common properties for any reflected element.
		if (!ReflectElement(reflField, cursor)) {
			return false;
		}

		const CXType parentType = clang_getCursorType(parent);
		const CXType fieldType = clang_getCursorType(cursor);

		// Build type info
		int compilerOffset = 0;
		if (!BuildTypeInfo(reflField.mTypeInfo, compilerOffset, fieldType)) {
			return false;
		}

		// Field offset
		const size_t offsetBits = clang_Type_getOffsetOf(parentType, reflField.mName.c_str());
		if (offsetBits % 8 != 0) {
			// TODO: Unsupported/tested at the moment.
			REFL_INTERNAL_RAISE_ERROR("Field [%s] has a bit offset not divisible by 8. This must be resolved internally.", reflField.mQualifiedName.c_str());
			return false;
		}
		reflField.mOffset = (offsetBits / 8) + stdFieldOffset;
		stdFieldOffset += compilerOffset;

		return true;
	}

	bool RegistryGenerator::ReflectFunction(Function& reflFunction, CXCursor cursor, bool isMemberFunction)
	{
		// Reflect common properties for any reflected element.
		if (!ReflectElement(reflFunction, cursor)) {
			return false;
		}

		// Return type
		const CXType methodType = clang_getCursorType(cursor);
		const CXType returnType = clang_getResultType(methodType);
		reflFunction.mReturnType = GetDataTypeFromClang(returnType);

		// Parameters
		const int numParams = clang_Cursor_getNumArguments(cursor);
		if (numParams > MAX_SUPPORTED_FUNCTION_PARAMETERS) {
			REFL_INTERNAL_RAISE_ERROR(
				"Tried to reflect function [%s] with (%d) parameters. Only a maximum of (%d) parameters are supported.",
				reflFunction.mQualifiedName.c_str(),
				numParams,
				MAX_SUPPORTED_FUNCTION_PARAMETERS);
			return false;
		}
		for (int i = 0; i < numParams; ++i) {
			const CXCursor paramCursor = clang_Cursor_getArgument(cursor, i);
			const CXType paramType = clang_getCursorType(paramCursor);

			TypeInfo paramTypeInfo;
			int nudge;
			if (BuildTypeInfo(paramTypeInfo, nudge, paramType)) {
				reflFunction.mParameterTypes.push_back(paramTypeInfo);
			}
		}

		reflFunction.mIsMemberFunction = isMemberFunction;

		return true;
	}

	// Reflects a class/struct/record.
	bool RegistryGenerator::ReflectClassMembers(Class& reflClass,int& stdFieldOffset,  CXCursor classCursor, std::vector<CXCursor> memberCursors)
	{
		// Collect all fields and functions in this class.
		for (auto memberCursor : memberCursors)
		{
			// Check for nested definitions
			if (memberCursor.kind == CXCursor_StructDecl) {
				const std::string name = GetCursorName(memberCursor);
				if (name == "") {
					// This is an un-named struct. Reflect everything as-is.
					if (!ReflectClassMembers(reflClass, stdFieldOffset, classCursor, CollectTopLevelChildren(memberCursor))) {
						return false;
					}
					continue;
				}
				else {
					Class nestedClassDefinition = ReflectClass(memberCursor);
					if (nestedClassDefinition == Class::INVALID) {
						return false;
					}
					continue;
				}
			}
			else if (memberCursor.kind == CXCursor_EnumDecl) {
				Enum reflEnum = ReflectEnum(memberCursor);
				if (reflEnum == Enum::INVALID) {
					return false;
				}
				continue;
			}

			// Don't reflect stuff we're not supposed to.
			if (!ShouldReflectCursor(memberCursor)) {
				continue;
			}

			if (memberCursor.kind == CXCursor_FieldDecl) {
				Field reflField;
				if (ReflectField(reflField, stdFieldOffset, memberCursor, classCursor)) {
					reflClass.mFields.push_back(reflField);
				}
			}
			else if (memberCursor.kind == CXCursor_CXXMethod) {
				Function reflFunction;
				if (ReflectFunction(reflFunction, memberCursor, true)) {
					reflClass.mFunctions.push_back(reflFunction);
				}
			}
			else if (memberCursor.kind == CXCursor_UnionDecl) {
				// nothing special needs to be done for named unions.
				// we're not going to actually reflect the union definition, we'll just reflect the actual union as a primitive blob type.
			}
			else {
				REFL_INTERNAL_RAISE_ERROR("Unhandled cursor type: %d", (int)memberCursor.kind);
			}
		}

		return true;
	}

	// Reflects a class/struct/record.
	Class RegistryGenerator::ReflectClass(CXCursor cursor)
	{
		Class reflClass;

		// Reflect common properties for any reflected element.
		if (!ReflectElement(reflClass, cursor)) {
			return Class::INVALID;
		}

		// Collect all fields and functions in this class.
		int stdFieldOffset = 0;
		if (!ReflectClassMembers(reflClass, stdFieldOffset, cursor, CollectTopLevelChildren(cursor))) {
			return Class::INVALID;
		}

		// Class size
		reflClass.mSize = clang_Type_getSizeOf(clang_getCursorType(cursor)) + stdFieldOffset;

		mRegistry->RegisterClass(reflClass);

		return reflClass;
	}

	bool RegistryGenerator::ReflectEnumValue(EnumValue& reflEnumValue, CXCursor cursor)
	{
		// Reflect common properties for any reflected element.
		if (!ReflectElement(reflEnumValue, cursor)) {
			return false;
		}

		reflEnumValue.mValue = (int)clang_getEnumConstantDeclValue(cursor);

		return true;
	}

	// Reflects an enum.
	Enum RegistryGenerator::ReflectEnum(CXCursor cursor)
	{
		Enum reflEnum;

		// Reflect common properties for any reflected element.
		if (!ReflectElement(reflEnum, cursor)) {
			return Enum::INVALID;
		}

		// Gather info about each value in this enum.
		std::vector<CXCursor> children = CollectTopLevelChildren(cursor);
		for (auto childCursor : children)
		{
			EnumValue reflEnumValue;
			if (ReflectEnumValue(reflEnumValue, childCursor)) {
				reflEnum.mValueTable[reflEnumValue.mValue] = reflEnumValue;
			}
		}

		mRegistry->RegisterEnum(reflEnum);

		return reflEnum;
	}

	bool RegistryGenerator::BuildReflection(CXCursor cursor)
	{
		for (auto child : CollectTopLevelChildren(cursor))
		{
			// Don't bother looking at types we don't support
			switch (child.kind)
			{
			case CXCursorKind::CXCursor_Namespace:
			case CXCursorKind::CXCursor_ClassDecl:
			case CXCursorKind::CXCursor_StructDecl:
			case CXCursorKind::CXCursor_EnumDecl:
			case CXCursorKind::CXCursor_FunctionDecl:
				break;

			default:
				continue;
			}

			// Should this cursor be reflected?
			if (!ShouldReflectCursor(child)) {
				continue;
			}

			// Do reflection parsing
			switch (child.kind)
			{
			case CXCursorKind::CXCursor_Namespace:
				BuildReflection(child);
				break;

			case CXCursorKind::CXCursor_ClassDecl:
			case CXCursorKind::CXCursor_StructDecl:
			{
				ReflectClass(child);
				break;
			}

			case CXCursorKind::CXCursor_EnumDecl:
			{
				ReflectEnum(child);
				break;
			}

			case CXCursorKind::CXCursor_FunctionDecl:
			{
				Function reflFunction;
				if (ReflectFunction(reflFunction, child, false)) {
					mRegistry->RegisterFunction(reflFunction);
				}
				break;
			}
			};
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	bool RegistryGenerator::Generate(Registry& outRegistry, GenerationParameters params)
	{
		mRegistry = &outRegistry;
		mParams = params;

		mIndex = clang_createIndex(0, 1);

		mTU = CreateTranslationUnit(mIndex);
		if (ReportClangErrors(mTU)) {
			return false;
		}

		BuildReflection(clang_getTranslationUnitCursor(mTU));

		// Cleanup
		clang_disposeTranslationUnit(mTU);
		clang_disposeIndex(mIndex);

		// finalize
		mRegistry->Finalize();

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	bool GenerateReflectionRegistry(Registry& outRegistry, const GenerationParameters& params)
	{
		RegistryGenerator generator;
		return generator.Generate(outRegistry, params);
	}
}