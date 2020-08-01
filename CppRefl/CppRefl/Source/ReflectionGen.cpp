#include "ReflectionGen.h"

#include <clang-c/Index.h>

#include "Reflection.h"

namespace refl
{
	//////////////////////////////////////////////////////////////////////////////

	// Report clang diagnostic errors.
	static bool reportClangError(CXTranslationUnit& TU)
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

	//////////////////////////////////////////////////////////////////////////////

	// Get a beautified name string for a cursor.
	static std::string getCursorName(CXCursor cursor)
	{
		CXString cxString = clang_getCursorSpelling(cursor);
		std::string name(clang_getCString(cxString));
		clang_disposeString(cxString);

		return name;
	}

	static std::string getFullyQualifiedName(CXCursor cursor)
	{
		std::string name = getCursorName(cursor);

		cursor = clang_getCursorSemanticParent(cursor);
		while (cursor.kind != CXCursor_FirstInvalid && cursor.kind != CXCursor_TranslationUnit)
		{
			name = getCursorName(cursor) + "::" + name;
			cursor = clang_getCursorSemanticParent(cursor);
		}

		return name;
	}

	// Collects all top level children cursors from a parent cursor.
	static std::vector<CXCursor> collectTopLevelChildren(CXCursor cursor)
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

	// Maps clang cursor types to Type's.
	static Type getTypeFromClang(CXTypeKind type)
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
			{ CXType_LongDouble,	Type::LONG_DOUBLE }
		};

		// Recognized type?
		if (Map.find(type) != Map.end()) {
			return Map[type];
		}

		return Type::INVALID;
	}

	// Maps clang cursor types to Type's.
	static Type getTypeFromClang(CXCursor cursor)
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

		return getTypeFromClang(cxType.kind);
	}

	// Decides whether or not to reflect a cursor.
	// e.g. Disregard anything outside of our project.
	static bool shouldReflectCursor(CXCursor cursor)
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
		// TODO: use custom attributes to decide when to generate reflection
		std::string projectPath = "X:\\projects\\CppRefl\\CppRefl\\CppRefl";
		if (filepath.rfind(projectPath, 0) != 0) {
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////

	// Reflects a field within a record.
	static Field reflectField(CXCursor cursor, CXCursor parent)
	{
		Field field;

		const CXType parentType = clang_getCursorType(parent);
		const CXType fieldType = clang_getCursorType(cursor);

		field.mName = getCursorName(cursor);
		field.mSize = clang_Type_getSizeOf(fieldType);
		field.mOffset = clang_Type_getOffsetOf(parentType, field.mName.c_str()) / 8;

		field.mIsPointer = fieldType.kind == CXType_Pointer;
		field.mIsConst = clang_isConstQualifiedType(fieldType);

		const CXCursor typeDeclaration = clang_getTypeDeclaration(fieldType);
		const std::string typeDeclarationQName = getFullyQualifiedName(typeDeclaration);

		field.mType = getTypeFromClang(cursor);

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
			field.mType = getTypeFromClang(elementType.kind);
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

	static Function reflectFunction(CXCursor cursor, CXCursor parent)
	{
		Function function;

		const CXType methodType = clang_getCursorType(cursor);

		// Return type
		/*const CXType returnType = clang_getResultType(methodType);
		if (returnType.kind == CXType_Void) {
			function.mReturnType = FunctionReturnType::VOID;
		}
		else if (returnType.kind == CXType_Bool) {
			function.mReturnType = FunctionReturnType::BOOL;
		}*/


		// Arguments
		int numArgs = clang_Cursor_getNumArguments(cursor);
		//clang_Cursor_getArgument(cursor, i);

		return function;
	}

	// Reflects a class/struct/record.
	static Class reflectClass(CXCursor cursor)
	{
		Class reflClass;

		reflClass.mName = getCursorName(cursor);
		reflClass.mQualifiedName = getFullyQualifiedName(cursor);

		std::vector<CXCursor> children = collectTopLevelChildren(cursor);
		for (auto childCursor : children)
		{
			if (childCursor.kind == CXCursor_FieldDecl) {
				reflClass.mFields.push_back(reflectField(childCursor, cursor));
			}
			else if (childCursor.kind == CXCursor_CXXMethod) {
				reflClass.mFunctions.push_back(reflectFunction(childCursor, cursor));
			}
		}

		return reflClass;
	}

	static EnumValue reflectEnumValue(CXCursor cursor)
	{
		EnumValue enumValue;

		enumValue.mName = getCursorName(cursor);
		enumValue.mQualifiedName = getFullyQualifiedName(cursor);
		enumValue.mValue = (int)clang_getEnumConstantDeclValue(cursor);

		return enumValue;
	}

	// Reflects an enum.
	static Enum reflectEnum(CXCursor cursor)
	{
		Enum reflEnum;

		reflEnum.mName = getCursorName(cursor);
		reflEnum.mQualifiedName = getFullyQualifiedName(cursor);

		std::vector<CXCursor> children = collectTopLevelChildren(cursor);
		for (auto childCursor : children)
		{
			EnumValue enumValue = reflectEnumValue(childCursor);
			reflEnum.mValueTable[enumValue.mValue] = enumValue;
		}

		return reflEnum;
	}

	// Builds reflection for a translation unit.
	static void buildReflection(Registry &registry, CXCursor cursor)
	{
		for (auto child : collectTopLevelChildren(cursor))
		{
			if (!shouldReflectCursor(child)) {
				continue;
			}

			switch (child.kind)
			{
			case CXCursorKind::CXCursor_Namespace:
				buildReflection(registry, child);
				break;

			case CXCursorKind::CXCursor_ClassDecl:
			case CXCursorKind::CXCursor_StructDecl:
				registry.ReflectClass(reflectClass(child));
				break;

			case CXCursorKind::CXCursor_EnumDecl:
				registry.ReflectEnum(reflectEnum(child));
				break;
			}
		}
	}


	bool GenerateReflectionRegistry(Registry& outRegistry, const char* inputFilepath, const char** clangArgs, int numClangArgs)
	{
		auto str = clang_getClangVersion();
		auto str2 = clang_getCString(str);
		printf("Clang Version: %s\n\n", str2);
		clang_disposeString(str);

		CXIndex index = clang_createIndex(0, 1);

		CXTranslationUnit TU = clang_createTranslationUnitFromSourceFile(index, inputFilepath, numClangArgs, clangArgs, 0, nullptr);
		if (TU == nullptr) {
			printf("Failed to parse input file: %s", inputFilepath);
			return false;
		}

		if (reportClangError(TU)) {
			return false;
		}

		buildReflection(outRegistry, clang_getTranslationUnitCursor(TU));

		// cleanup
		clang_disposeTranslationUnit(TU);
		clang_disposeIndex(index);

		return true;
	}
}