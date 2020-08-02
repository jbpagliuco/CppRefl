#include "ReflectionGen.h"

#include <clang-c/Index.h>

#include "Reflection.h"

namespace refl
{
	static GenerationParameters GenerationParams;

	//////////////////////////////////////////////////////////////////////////////

	static void PrintClangVersion()
	{
		auto str = clang_getClangVersion();
		auto str2 = clang_getCString(str);
		printf("Clang Version: %s\n\n", str2);
		clang_disposeString(str);
	}

	// Report clang diagnostic errors.
	static bool ReportClangError(CXTranslationUnit& TU)
	{
		bool errors = false;

		CXDiagnosticSet diagnosticSet = clang_getDiagnosticSetFromTU(TU);
		const int numDiagnostics = clang_getNumDiagnosticsInSet(diagnosticSet);
		for (int i = 0; i < numDiagnostics; ++i) {
			CXDiagnostic diagnostic = clang_getDiagnosticInSet(diagnosticSet, i);

			CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
			if (severity >= CXDiagnosticSeverity::CXDiagnostic_Warning) {
				errors = true;
			}

			printf("CLANG ERRORS\n");
		}

		fflush(stdout);

		return errors;
	}

	static CXTranslationUnit CreateTranslationUnit(CXIndex index, const std::string& inputFilepath, const std::vector<std::string>& clangArgs, const std::vector<std::string>& includePaths)
	{
		std::vector<std::string> includeArgs;
		for (const auto& path : includePaths) {
			includeArgs.push_back("-I" + path);
		}

		std::vector<const char*> allArgs = {
			"-DCPP_REFL_BUILD_REFLECTION", // Creates a CPP_REFL_BUILD_REFLECTION macro
		};

		for (const auto& arg : clangArgs) {
			allArgs.push_back(const_cast<char*>(arg.c_str()));
		}

		for (const auto& arg : includeArgs) {
			allArgs.push_back(const_cast<char*>(arg.c_str()));
		}

		return clang_createTranslationUnitFromSourceFile(index, inputFilepath.c_str(), (int)allArgs.size(), &allArgs[0], 0, nullptr);
	}

	//////////////////////////////////////////////////////////////////////////////

	// Get a beautified name string for a cursor.
	static std::string GetCursorName(CXCursor cursor)
	{
		CXString cxString = clang_getCursorSpelling(cursor);
		std::string name(clang_getCString(cxString));
		clang_disposeString(cxString);

		return name;
	}

	// Get a fully qualified name string (including namespaces) for a cursor.
	static std::string GetFullyQualifiedName(CXCursor cursor)
	{
		std::string name = GetCursorName(cursor);

		cursor = clang_getCursorSemanticParent(cursor);
		while (cursor.kind != CXCursor_FirstInvalid && cursor.kind != CXCursor_TranslationUnit)
		{
			name = GetCursorName(cursor) + "::" + name;
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

	// Collect annotations from a cursor.
	static std::vector<std::string> CollectCursorAnnotations(CXCursor cursor)
	{
		auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData clientData)
		{
			std::vector<std::string>* children = static_cast<std::vector<std::string>*>(clientData);

			// Collect all the annotations that contain our custom reflection markup
			if (cursor.kind == CXCursor_AnnotateAttr) {
				const std::string annotation = GetCursorName(cursor);
				if (annotation.find(CPP_REFLECTION_ANNOTATION) != std::string::npos) {
					children->push_back(annotation);
				}
			}
			return CXChildVisit_Continue;
		};

		std::vector<std::string> children;
		clang_visitChildren(cursor, visitor, &children);

		return children;
	}

	// Maps clang cursor types to refl::Type.
	static Type GetTypeFromClang(CXTypeKind type)
	{
		static std::map<CXTypeKind, Type> Map = {
			{ CXType_Bool,			Type::BOOL },
			{ CXType_Char_U,		Type::UINT8 },
			{ CXType_UChar,			Type::UINT8 },
			{ CXType_Char16,		Type::INT16 },
			{ CXType_Char32,		Type::INT32 },
			{ CXType_UShort,		Type::UINT16 },
			{ CXType_UInt,			Type::UINT32 },
			{ CXType_ULong,			Type::UINT32 },
			{ CXType_ULongLong,		Type::UINT64 },
			{ CXType_Char_S,		Type::INT8 },
			{ CXType_SChar,			Type::INT8 },
			{ CXType_WChar,			Type::INT16 },
			{ CXType_Short,			Type::INT16},
			{ CXType_Int,			Type::INT32 },
			{ CXType_Long,			Type::INT32 },
			{ CXType_LongLong,		Type::INT64 },
			{ CXType_Float,			Type::FLOAT },
			{ CXType_Double,		Type::DOUBLE },
			{ CXType_LongDouble,	Type::LONG_DOUBLE },
			{ CXType_Void,			Type::VOID },
		};

		// Recognized type?
		if (Map.find(type) != Map.end()) {
			return Map[type];
		}

		return Type::INVALID;
	}

	// Maps clang cursor types to Type's.
	static Type GetTypeFromClang(CXCursor cursor)
	{
		CXType cxType = clang_getCursorType(cursor);

		CXType pointeeType = clang_getPointeeType(cxType);
		if (pointeeType.kind != CXType_Invalid)
		{
			cxType = pointeeType;
		}

		// See if this cursor can be translated into a different decl
		CXCursor translatedCursor = clang_getTypeDeclaration(cxType);
		if (translatedCursor.kind != CXCursor_NoDeclFound)
		{
			// Is this a record decl?
			if (translatedCursor.kind == CXCursor_StructDecl || translatedCursor.kind == CXCursor_ClassDecl)
			{
				return Type::CLASS;
			}

			// Do typedef translation
			if (cxType.kind == CXType_Typedef)
			{
				cxType = clang_getTypedefDeclUnderlyingType(translatedCursor);
			}

			// Do enum translation
			if (cxType.kind == CXType_Enum)
			{
				cxType = clang_getEnumDeclIntegerType(translatedCursor);
			}
		}

		return GetTypeFromClang(cxType.kind);
	}

	// Decides whether or not to reflect a cursor.
	// e.g. Disregard anything outside of our project.
	static bool ShouldReflectCursor(CXCursor cursor)
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
		if (GenerationParams.mProjectPath != "" && filepath.rfind(GenerationParams.mProjectPath, 0) != 0) {
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

	//////////////////////////////////////////////////////////////////////////////

	// Gathers common reflection information for any refl::Element type.
	static void ReflectElement(Element& reflElement, CXCursor cursor)
	{
		// Names
		reflElement.mName = GetCursorName(cursor);
		reflElement.mQualifiedName = GetFullyQualifiedName(cursor);

		// Attributes
		for (auto annotation : CollectCursorAnnotations(cursor)) {
			// Attributes are split by a special separator. e.g.:
			// annotate(cpp_refl,just_a_tag)
			// annotate(cpp_ref,tag_with_a_value,the_value)
			const size_t firstSeparator = annotation.find(CPP_REFLECTION_SEPARATOR);

			// Is this actually an attribute?
			if (firstSeparator == std::string::npos) {
				continue;
			}

			const std::string metadata = annotation.substr(firstSeparator + 1);

			// If this separator doesn't exist, this is just a tag with no value
			const size_t secondSeparator = metadata.find(CPP_REFLECTION_SEPARATOR);
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
	}

	// Reflects a field within a record.
	static Field ReflectField(CXCursor cursor, CXCursor parent)
	{
		Field field;

		// Reflect common properties for any reflected element.
		ReflectElement(field, cursor);

		const CXType parentType = clang_getCursorType(parent);
		const CXType fieldType = clang_getCursorType(cursor);

		field.mSize = clang_Type_getSizeOf(fieldType);
		field.mOffset = clang_Type_getOffsetOf(parentType, field.mName.c_str()) / 8;

		field.mIsPointer = fieldType.kind == CXType_Pointer;
		field.mIsConst = clang_isConstQualifiedType(fieldType);

		const CXCursor typeDeclaration = clang_getTypeDeclaration(fieldType);
		const std::string typeDeclarationQName = GetFullyQualifiedName(typeDeclaration);

		field.mType = GetTypeFromClang(cursor);

		// Special container types
		if (typeDeclarationQName == "std::string")
		{
			field.mIsString = true;
			field.mClassType = typeDeclarationQName;
			field.mType = Type::CLASS;
		}
		else if (typeDeclarationQName == "std::vector")
		{
			field.mIsArray = true;
			field.mClassType = typeDeclarationQName;
			field.mType = Type::CLASS;

			// Get the vector element type
			CXType elementType = clang_Type_getTemplateArgumentAsType(fieldType, 0);
			field.mType = GetTypeFromClang(elementType.kind);
			field.mSize = clang_Type_getSizeOf(elementType);
		}

		// Non primitive types
		if (field.mType == Type::CLASS)
		{
			field.mClassType = typeDeclarationQName;
		}
		else if (fieldType.kind == CXType_Enum)
		{
			field.mEnumType = typeDeclarationQName;
		}

		return field;
	}

	static Function ReflectFunction(CXCursor cursor, CXCursor parent)
	{
		Function function;

		// Reflect common properties for any reflected element.
		ReflectElement(function, cursor);


		// Return type
		const CXType methodType = clang_getCursorType(cursor);
		const CXType returnType = clang_getResultType(methodType);
		function.mReturnType = GetTypeFromClang(returnType.kind);

		// Arguments
		int numArgs = clang_Cursor_getNumArguments(cursor);
		//clang_Cursor_getArgument(cursor, i);

		return function;
	}

	// Reflects a class/struct/record.
	static Class ReflectClass(CXCursor cursor)
	{
		Class reflClass;

		// Reflect common properties for any reflected element.
		ReflectElement(reflClass, cursor);

		// Collect all fields and functions in this class.
		std::vector<CXCursor> children = CollectTopLevelChildren(cursor);
		for (auto childCursor : children)
		{
			// Don't reflect stuff we're not supposed to.
			if (!ShouldReflectCursor(childCursor)) {
				continue;
			}

			if (childCursor.kind == CXCursor_FieldDecl) {
				reflClass.mFields.push_back(ReflectField(childCursor, cursor));
			}
			else if (childCursor.kind == CXCursor_CXXMethod) {
				reflClass.mFunctions.push_back(ReflectFunction(childCursor, cursor));
			}
		}

		return reflClass;
	}

	static EnumValue ReflectEnumValue(CXCursor cursor)
	{
		EnumValue enumValue;

		// Reflect common properties for any reflected element.
		ReflectElement(enumValue, cursor);

		enumValue.mValue = (int)clang_getEnumConstantDeclValue(cursor);

		return enumValue;
	}

	// Reflects an enum.
	static Enum ReflectEnum(CXCursor cursor)
	{
		Enum reflEnum;

		// Reflect common properties for any reflected element.
		ReflectElement(reflEnum, cursor);

		// Gather info about each value in this enum.
		std::vector<CXCursor> children = CollectTopLevelChildren(cursor);
		for (auto childCursor : children)
		{
			EnumValue enumValue = ReflectEnumValue(childCursor);
			reflEnum.mValueTable[enumValue.mValue] = enumValue;
		}

		return reflEnum;
	}

	// Builds reflection for a translation unit.
	static void BuildReflection(Registry &registry, CXCursor cursor)
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
				BuildReflection(registry, child);
				break;

			case CXCursorKind::CXCursor_ClassDecl:
			case CXCursorKind::CXCursor_StructDecl:
				registry.RegisterClass(ReflectClass(child));
				break;

			case CXCursorKind::CXCursor_EnumDecl:
				registry.RegisterEnum(ReflectEnum(child));
				break;
			}
		}
	}


	bool GenerateReflectionRegistry(Registry& outRegistry, const GenerationParameters& params)
	{
		GenerationParams = params;

		PrintClangVersion();

		CXIndex index = clang_createIndex(0, 1);

		CXTranslationUnit TU = CreateTranslationUnit(index, params.mInputFilepath, params.mClangArgs, params.mIncludePaths);
		if (ReportClangError(TU)) {
			return false;
		}

		BuildReflection(outRegistry, clang_getTranslationUnitCursor(TU));

		// cleanup
		clang_disposeTranslationUnit(TU);
		clang_disposeIndex(index);

		// finalize
		outRegistry.Finalize();

		return true;
	}
}