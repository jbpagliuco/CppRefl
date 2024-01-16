using ClangSharp.Interop;
using CppRefl.CodeGeneration;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler;

public record CompilerParams
{
	// Entrypoint of the program. Should be a .cpp of .h file.
	public required FileInfo InputFile { get; init; }

	// Name of the module we're generating reflection for (Game, Engine, Core, etc.)
	public required string ModuleName { get; init; }

	// Base file path of the module. Any symbols defined outside of this filepath are ignored.
	public required DirectoryInfo ModuleDirectory { get; init; }

	// List of arguments to pass to clang.
	public IEnumerable<string> ClangArgs { get; init; } = Enumerable.Empty<string>();

	// List of include paths for the program.
	public IEnumerable<string> IncludePaths { get; init; } = Enumerable.Empty<string>();

	// List of preprocessor defines for the program.
	public IEnumerable<string> Definitions { get; init; } = Enumerable.Empty<string>();

	// Raise clang warnings during compilation (warings will result in reflection data not being generated).
	public bool RaiseClangWarnings { get; init; } = true;

	// Raise clang errors during compilation (errors will result in reflection data not being generated).
	public bool RaiseClangErrors { get; init; } = true;

	// Output directory containing generated code.
	public required DirectoryInfo OutputDirectory { get; init; }

	// Deletes empty header files after compilation.
	public bool DeleteEmptyHeaderFiles { get; set; } = false;

	// Deletes empty source files after compilation.
	public bool DeleteEmptySourceFiles { get; set; } = true;

	// Deletes empty directories after compilation.
	public bool DeleteEmptyDirectories { get; set; } = true;
}

internal record TemplatedFieldInfo
{
	public required IList<TypeInfo> SpecializedTemplateArguments { get; init; }
	public required IList<TypeInfo> GenericTemplateArguments { get; init; }
	public required IList<FieldInfo> Fields { get; init; }
}

public class Compiler
{
	public Compiler(CompilerParams @params)
	{
		Params = @params;
		
		ClangArgs = DefaultClangArgs
			.Concat(@params.ClangArgs)
			.Concat(@params.IncludePaths.Where(x => x != "").Select(x => $"-I{x}"))
			.Concat(@params.Definitions.Where(x => x != "").Select(x => $"-D{x}"))
			.Append($"-D{CppDefines.BuildReflection}")
			.ToArray();

		OutputFiles = new[]
		{
			CodeGenerator.SourceToGenerated(Params.InputFile, Params.ModuleDirectory, Params.OutputDirectory, CodeGenerator.GeneratedHeaderExtension),
			CodeGenerator.SourceToGenerated(Params.InputFile, Params.ModuleDirectory, Params.OutputDirectory, CodeGenerator.GeneratedSourceExtension),
			CodeGenerator.SourceToGenerated(Params.InputFile, Params.ModuleDirectory, Params.OutputDirectory, Registry.RegistryExtension),
		};
	}

	/// <summary>
	/// Parameters of this compiler instance.
	/// </summary>
	public CompilerParams Params { get; }

	/// <summary>
	/// Reflection registry.
	/// </summary>
	public Registry Registry { get; } = new();

	/// <summary>
	/// Clang arguments.
	/// </summary>
	public string[] ClangArgs { get; }

	/// <summary>
	/// Default clang arguments.
	/// </summary>
	public static IReadOnlyCollection<string> DefaultClangArgs { get; } = new[]
	{
		"-fms-compatibility",
		"-fms-compatibility-version=19.26",
		"-fms-extensions",
		"-Wall",
		"-Wmicrosoft",
		"-Wno-c++98-compat",
		"-Wno-c++98-compat-pedantic",
		"-Wno-newline-eof",
		"-Wno-pragma-once-outside-header",
		"-Wno-unused-variable",
		"--language=c++",
	};

	/// <summary>
	/// Clang index.
	/// </summary>
	private CXIndex Index { get; set; }

	/// <summary>
	/// Clang index.
	/// </summary>
	private CXTranslationUnit TranslationUnit { get; set; }

	/// <summary>
	/// List of output files that will be generated.
	/// </summary>
	private FileInfo[] OutputFiles { get; }

	/// <summary>
	/// Mapping of TypeInfo data to their original cursor.
	/// </summary>
	private IDictionary<TypeInfo, CXCursor> TypeCursors { get; } = new Dictionary<TypeInfo, CXCursor>();

	/// <summary>
	/// Generates empty files that are included for reflection.
	/// </summary>
	public void GenerateEmptyFiles()
	{
		Params.OutputDirectory.Create();

		// Delete our specific files (we'll recreate them after this).
		foreach (var file in OutputFiles)
		{
			file.Delete();
		}

		// Create empty "reflgen.h" files for everything.
		var headerFiles = Params.ModuleDirectory.EnumerateFiles("*.h", SearchOption.AllDirectories);
		foreach (var headerFile in headerFiles)
		{
			if (headerFile.Extension != CodeGenerator.GeneratedHeaderExtension)
			{
				var outputFile = CodeGenerator.SourceToGenerated(headerFile, Params.ModuleDirectory, Params.OutputDirectory, CodeGenerator.GeneratedHeaderExtension);
				if (!outputFile.Exists)
				{
					outputFile.Directory!.Create();

					try
					{
						outputFile.CreateText().Close();
					}
					catch (IOException)
					{
					}
				}
			}
		}
	}

	/// <summary>
	/// Generates the reflection registry of the given program.
	/// </summary>
	public void GenerateRegistry()
	{
		GenerateEmptyFiles();

		Index = CXIndex.Create();
		TranslationUnit = ClangUtils.CreateTranslationUnit(Index, Params.InputFile.FullName, ClangArgs, Params.RaiseClangWarnings, Params.RaiseClangErrors);

		ClangUtils.VisitCursorChildren(TranslationUnit.Cursor, ReflectCursor);
	}

	/// <summary>
	/// Cleans up files with the given extension.
	/// </summary>
	/// <param name="extension"></param>
	private void CleanupFiles(string extension)
	{
		foreach (var file in Directory.EnumerateFiles(Params.OutputDirectory.FullName, $"*{extension}"))
		{
			var fileInfo = new FileInfo(file);
			if (fileInfo.Exists && fileInfo.Length == 0)
			{
				File.Delete(file);
			}
		}
	}

	/// <summary>
	/// Deletes pre-generated files that don't actually contain any reflection data.
	/// </summary>
	public void Cleanup()
	{
		if (Params.DeleteEmptyHeaderFiles)
		{
			CleanupFiles(CodeGenerator.GeneratedHeaderExtension);
		}

		if (Params.DeleteEmptySourceFiles)
		{
			CleanupFiles(CodeGenerator.GeneratedSourceExtension);
		}

		if (Params.DeleteEmptyDirectories)
		{
			foreach (var dir in Directory.EnumerateDirectories(Params.OutputDirectory.FullName))
			{
				var dirInfo = new DirectoryInfo(dir);
				if (dirInfo.GetFiles().Length == 0)
				{
					Directory.Delete(dir);
				}
			}
		}
	}

	/// <summary>
	/// Reflect a cursor.
	/// </summary>
	/// <param name="cursor"></param>
	private CXChildVisitResult ReflectCursor(CXCursor cursor)
	{
		string spelling = cursor.Spelling.ToString();

		switch (cursor.Kind)
		{
			// These are the only types we care to reflect.
			case CXCursorKind.CXCursor_TranslationUnit:
			case CXCursorKind.CXCursor_Namespace:
				return CXChildVisitResult.CXChildVisit_Recurse;

			case CXCursorKind.CXCursor_ClassDecl:
			case CXCursorKind.CXCursor_StructDecl:
			case CXCursorKind.CXCursor_ClassTemplate:
			case CXCursorKind.CXCursor_EnumDecl:
			case CXCursorKind.CXCursor_TypeAliasDecl:
				if (!cursor.IsDefinition)
				{
					// Skip forward declarations.
					return CXChildVisitResult.CXChildVisit_Continue;
				}
				break;

			case CXCursorKind.CXCursor_FunctionDecl:
				break;

			default:
				return CXChildVisitResult.CXChildVisit_Continue;
		}

		// Outside of our module entirely, ignore it.
		if (!IsCursorWithinModule(cursor))
		{
			return CXChildVisitResult.CXChildVisit_Continue;
		}

		// Collect metadata (and make sure this is actually reflected).
		var metadata = ClangUtils.GetReflectedCursorMetadata(cursor);
		if (metadata == null)
		{
			return CXChildVisitResult.CXChildVisit_Continue;
		}
		
		// Reflect this type.
		switch (cursor.Kind)
		{
			case CXCursorKind.CXCursor_ClassDecl:
			case CXCursorKind.CXCursor_StructDecl:
			case CXCursorKind.CXCursor_ClassTemplate:
				ReflectClass(cursor, metadata);
				break;

			case CXCursorKind.CXCursor_EnumDecl:
				ReflectEnum(cursor, metadata);
				break;

			case CXCursorKind.CXCursor_TypeAliasDecl:
				ReflectAlias(cursor, metadata);
				break;

			case CXCursorKind.CXCursor_FunctionDecl:
				ReflectFunction(cursor, metadata);
				break;

			default:
				break;
		}

		// Visit children.
		ClangUtils.VisitCursorChildren(cursor, ReflectCursor);
		return CXChildVisitResult.CXChildVisit_Continue;
	}

	/// <summary>
	/// Returns whether or not the given cursor belongs to our input module.
	/// </summary>
	/// <param name="cursor"></param>
	/// <returns></returns>
	private bool IsCursorWithinModule(CXCursor cursor)
	{
		// Get the file info of where this is declared.
		cursor.Location.GetSpellingLocation(out var file, out var _, out var _, out var _);

		// Exclude anything outside of the given module.
		var filename = Path.GetFullPath(file.ToString());
		if (!filename.StartsWith(Params.ModuleDirectory.FullName))
		{
			return false;
		}

		return true;
	}

	/// <summary>
	/// Reflect a type.
	/// </summary>
	/// <param name="clangType"></param>
	/// <returns></returns>
	private TypeInfo ReflectType(CXType clangType)
	{
		bool isTemplateParam = clangType.TypeClass == CX_TypeClass.CX_TypeClass_TemplateTypeParm;
		bool isTypedef = clangType.kind == CXTypeKind.CXType_Typedef;

		// Use the underlying type.
		if (!isTemplateParam && !isTypedef)
		{
			clangType = clangType.CanonicalType;
		}
		
		clangType = clangType.ArrayElementType.kind != CXTypeKind.CXType_Invalid ? clangType.ArrayElementType : clangType;

		ClangUtils.GetNameInfo(clangType.UnqualifiedType, out var qualifiedName, out var name, out var @namespace);

		var typeInfo = Registry.GetType(qualifiedName);
		if (typeInfo == null)
		{
			var templateArguments = ClangUtils.GetTemplateArguments(clangType.CanonicalType).Select(ReflectType).ToList();

			typeInfo = new TypeInfo()
			{
				Name = name,
				Namespace = @namespace,
				Kind = ClangUtils.GetTypeKind(clangType.CanonicalType),
				Template = !templateArguments.Any() ? null : new()
				{
					Arguments = templateArguments,
					IsSpecialized = true
				}
			};
			Registry.AddType(typeInfo);

			TypeCursors[typeInfo] = clangType.Declaration;
		}

		return typeInfo;
	}

	/// <summary>
	/// Reflect a type.
	/// </summary>
	/// <param name="cursor"></param>
	/// <returns></returns>
	private TypeInfo ReflectType(CXCursor cursor)
	{
		if (cursor.Type.kind != CXTypeKind.CXType_Invalid)
		{
			return ReflectType(cursor.Type);
		}

		if (!cursor.IsTemplated)
		{
			throw new NotImplementedException("Unknown edge case.");
		}

		ClangUtils.GetNameInfo(cursor, out var qualifiedName, out var name, out var @namespace);

		var typeInfo = Registry.GetType(qualifiedName);
		if (typeInfo == null)
		{
			var templateArguments = ClangUtils.GetTemplateArguments(cursor).Select(ReflectType).ToList();

			typeInfo = new TypeInfo
			{
				Name = name,
				Namespace = ClangUtils.GetNamespace(cursor),
				Kind = TypeKind.Class,
				Template = !templateArguments.Any() ? null : new()
				{
					Arguments = templateArguments,
					IsSpecialized = false
				}
			};

			Registry.AddType(typeInfo);

			TypeCursors[typeInfo] = cursor;
		}

		return typeInfo;
	}

	/// <summary>
	/// Reflects a type instance.
	/// </summary>
	/// <param name="clangType"></param>
	/// <returns></returns>
	private TypeInstanceInfo ReflectTypeInstance(CXType clangType)
	{
		return new TypeInstanceInfo()
		{
			Type = ReflectType(clangType),
			Const = clangType.IsConstQualified
		};
	}

	/// <summary>
	/// Reflect common data on a function/method.
	/// </summary>
	/// <param name="cursor"></param>
	/// <param name="returnType"></param>
	/// <param name="parameterTypes"></param>
	/// <returns></returns>
	private void ReflectFunctionCommon(CXCursor cursor, out TypeInstanceInfo returnType, out IList<TypeInstanceInfo> parameterTypes)
	{
		returnType = ReflectTypeInstance(cursor.Type.ResultType);

		parameterTypes = new List<TypeInstanceInfo>();
		for (uint i = 0; i < cursor.NumArguments; ++i)
		{
			CXCursor parameterCursor = cursor.GetArgument(i);
			CXType parameterType = parameterCursor.Type;
			parameterTypes.Add(ReflectTypeInstance(parameterType));
		}
	}

	/// <summary>
	/// Reflects a field on a class.
	/// </summary>
	/// <param name="cursor"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	private FieldInfo ReflectField(CXCursor cursor, MetadataInfo metadata)
	{
		var fieldInfo = new FieldInfo()
		{
			Name = cursor.Spelling.ToString(),
			TypeInstance = ReflectTypeInstance(cursor.Type),
			Metadata = metadata
		};

		return fieldInfo;
	}

	/// <summary>
	/// Reflects a method on a class.
	/// </summary>
	/// <param name="cursor"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	private MethodInfo ReflectMethod(CXCursor cursor, MetadataInfo metadata)
	{
		ReflectFunctionCommon(cursor, out var returnType, out var parameterTypes);

		var methodInfo = new MethodInfo()
		{
			Name = cursor.Spelling.ToString(),
			ReturnType = returnType,
			ArgumentTypes = parameterTypes,
			Metadata = metadata
		};

		return methodInfo;
	}

	/// <summary>
	/// Reflects a class member.
	/// </summary>
	/// <param name="cursor"></param>
	/// <param name="classInfo"></param>
	/// <returns></returns>
	/// <exception cref="NotImplementedException"></exception>
	private CXChildVisitResult ReflectClassMember(CXCursor cursor, ClassInfo classInfo)
	{
		// See if this is the GENERATED_REFLECTION_CODE() macro. We need the line number to properly generate the class body.
		if (cursor.Kind == CXCursorKind.CXCursor_CXXMethod && cursor.Spelling.ToString() == CppDefines.GeneratedReflectionCodeMarker)
		{
			cursor.Location.GetFileLocation(out var _, out var line, out var _, out var _);
			classInfo.GeneratedBodyLine = line;
			return CXChildVisitResult.CXChildVisit_Continue;
		}

		// Collect base classes.
		if (cursor.Kind == CXCursorKind.CXCursor_CXXBaseSpecifier)
		{
			var baseClassCursor = cursor.Type.CanonicalType.Declaration;
			ClassInfo? baseClassInfo;
			switch (baseClassCursor.Kind)
			{
				// (Non specialized) templated base class.
				case CXCursorKind.CXCursor_ClassTemplate:
					baseClassInfo = ReflectClass(baseClassCursor, ClangUtils.GetCursorMetadata(baseClassCursor));
					break;

				// Instantiable base class.
				case CXCursorKind.CXCursor_ClassDecl:
				case CXCursorKind.CXCursor_StructDecl:
					baseClassInfo = ReflectClass(cursor.Type.CanonicalType);
					break;

				default:
					throw new NotImplementedException();
			}

			classInfo.BaseClasses.Add(baseClassInfo);
		}

		var metadata = ClangUtils.GetReflectedCursorMetadata(cursor);
		if (metadata == null)
		{
			return CXChildVisitResult.CXChildVisit_Continue;
		}

		switch (cursor.Kind)
		{
			case CXCursorKind.CXCursor_FieldDecl:
				if (!cursor.IsTemplated || classInfo.Type.Template!.IsGeneric)
				{
					classInfo.Fields.Add(ReflectField(cursor, metadata));
				}

				break;

			case CXCursorKind.CXCursor_CXXMethod:
				classInfo.Methods.Add(ReflectMethod(cursor, metadata));
				break;

			default:
				throw new NotImplementedException($"Unhandled class member type: {cursor.Kind}");
		}

		return CXChildVisitResult.CXChildVisit_Continue;
	}

	/// <summary>
	/// Get all fields on templated base classes.
	/// </summary>
	/// <param name="classInfo">The base class to start collecting from.</param>
	/// <returns></returns>
	private IList<TemplatedFieldInfo> GetTemplateBaseClassFields(ClassInfo? classInfo, IList<TypeInfo>? specializedTemplateArguments = null)
	{
		IList<TemplatedFieldInfo> fields = new List<TemplatedFieldInfo>();

		if (classInfo?.Type.IsTemplated == true)
		{
			if (classInfo.Type.Template!.IsGeneric)
			{
				// Generic base class.
				fields.Add(new ()
				{
					SpecializedTemplateArguments = specializedTemplateArguments ?? throw new ArgumentNullException(),
					GenericTemplateArguments = classInfo.Type.Template.Arguments,
					Fields = classInfo.Fields
				});

				fields = fields.Concat(GetTemplateBaseClassFields(classInfo.BaseClasses.FirstOrDefault(), specializedTemplateArguments)).ToList();
			}
			else
			{
				// Specialized base class. Go find the original template type.
				string templateType = classInfo.Type.TemplateType!;
				ClassInfo templateClass = Registry.GetClass(templateType)!;
				return GetTemplateBaseClassFields(templateClass, classInfo.Type.Template.Arguments);
			}
		}

		return fields;
	}

	private void AddTemplateBaseClassFields(ClassInfo classInfo)
	{
		// Scoop up all the reflected fields on templated base classes.
		if (classInfo.Type.Template?.IsGeneric != true)
		{
			// Traverse into base classes (if any).
			var templatedFieldInfos = classInfo.Type.Template?.IsSpecialized == true 
				? GetTemplateBaseClassFields(Registry.GetClass(classInfo.Type.TemplateType!)!, classInfo.Type.Template.Arguments)
				: GetTemplateBaseClassFields(classInfo.BaseClasses.FirstOrDefault());
			
			foreach (var it in templatedFieldInfos)
			{
				foreach (var fieldInfo in it.Fields)
				{
					if (fieldInfo.Type.Kind == TypeKind.Template)
					{
						int templateArgIndex = it.GenericTemplateArguments.IndexOf(fieldInfo.Type);
						classInfo.Fields.Add(new FieldInfo()
						{
							Name = fieldInfo.Name,
							Metadata = fieldInfo.Metadata,
							TypeInstance = new()
							{
								Const = fieldInfo.TypeInstance.Const,
								Type = it.SpecializedTemplateArguments[templateArgIndex]
							}
						});
					}
					else
					{
						classInfo.Fields.Add(fieldInfo);
					}
				}

			}
		}
	}

	/// <summary>
	/// Reflect a template class specialization.
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	private ClassInfo ReflectClass(CXType type)
	{
		ClangUtils.GetNameInfo(type, out var qualifiedName, out var name, out var @namespace);

		var classInfo = Registry.GetClass(qualifiedName);
		if (classInfo == null)
		{
			classInfo = new()
			{
				Type = ReflectType(type),
				Metadata = ClangUtils.GetCursorMetadata(type.Declaration),
				ClassType = ClangUtils.GetClassType(type.Declaration),
				IsAbstract = type.Declaration.CXXRecord_IsAbstract
			};

			CXCursor cursor = type.Declaration.CanonicalCursor;
			if (classInfo.Type.Template?.IsSpecialized == true)
			{
				var templateType = Registry.GetType(classInfo.Type.TemplateType!);
				if (templateType == null)
				{
					throw new ArgumentNullException($"Template type for '{classInfo.Type.TemplateType}' was not found. Is it reflected?");
				}
				cursor = TypeCursors[templateType];
			}

			ClangUtils.VisitCursorChildren(cursor, c => ReflectClassMember(c, classInfo));

			// Reset this field.
			if (classInfo.Type.Template?.IsSpecialized == true)
			{
				classInfo.GeneratedBodyLine = null;
			}

			AddTemplateBaseClassFields(classInfo);

			Registry.AddClass(classInfo);
		}

		return classInfo;
	}

	/// <summary>
	/// Reflect a class.
	/// </summary>
	/// <param name="cursor"></param>
	/// <param name="metadata"></param>
	private ClassInfo ReflectClass(CXCursor cursor, MetadataInfo metadata)
	{
		ClangUtils.GetNameInfo(cursor, out var qualifiedName, out var name, out var @namespace);

		var classInfo = Registry.GetClass(qualifiedName);
		if (classInfo == null)
		{
			classInfo = new()
			{
				Type = ReflectType(cursor),
				Metadata = metadata,
				ClassType = ClangUtils.GetClassType(cursor),
				IsAbstract = cursor.CXXRecord_IsAbstract
			};
			
			ClangUtils.VisitCursorChildren(cursor, c => ReflectClassMember(c, classInfo));

			AddTemplateBaseClassFields(classInfo);

			Registry.AddClass(classInfo);
		}

		return classInfo;
	}

	/// <summary>
	/// Reflect an enum.
	/// </summary>
	/// <param name="cursor"></param>
	/// <returns></returns>
	private EnumValueInfo ReflectEnumValue(CXCursor cursor)
	{
		var enumValueInfo = new EnumValueInfo()
		{
			Name = cursor.Spelling.ToString(),
			Value = cursor.EnumConstantDeclValue,
			Metadata = ClangUtils.GetCursorMetadata(cursor)
		};

		return enumValueInfo;
	}

	/// <summary>
	/// Reflect an enum.
	/// </summary>
	/// <param name="cursor"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	private EnumInfo ReflectEnum(CXCursor cursor, MetadataInfo metadata)
	{
		string name = ClangUtils.GetFullyQualifiedType(cursor);

		var enumInfo = Registry.GetEnum(name);
		if (enumInfo == null)
		{
			enumInfo = new()
			{
				Type = ReflectType(cursor.Type),
				Metadata = metadata
			};

			CXChildVisitResult Visitor(CXCursor child)
			{
				if (child.Kind == CXCursorKind.CXCursor_EnumConstantDecl)
				{
					enumInfo.Values.Add(ReflectEnumValue(child));
				}

				return CXChildVisitResult.CXChildVisit_Continue;
			}
			ClangUtils.VisitCursorChildren(cursor, Visitor);

			Registry.AddEnum(enumInfo);
		}

		return enumInfo;
	}

	/// <summary>
	/// Reflect an alias.
	/// </summary>
	/// <param name="cursor"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	private AliasInfo ReflectAlias(CXCursor cursor, MetadataInfo metadata)
	{
		string name = ClangUtils.GetFullyQualifiedType(cursor);
		var aliasInfo = Registry.GetAlias(name);
		if (aliasInfo == null)
		{
			var aliasedType = cursor.Type.CanonicalType;
			var aliasedName = ClangUtils.GetFullyQualifiedType(aliasedType);

			ClassInfo? aliasClassInfo = null;
			EnumInfo? aliasEnumInfo = null;
			switch (aliasedType.Declaration.Type.kind)
			{
				case CXTypeKind.CXType_Record:
					aliasClassInfo = Registry.GetClass(aliasedName);
					
					// Special case where we're aliasing a templated specialization.
					if (aliasClassInfo == null && aliasedType.NumTemplateArguments > 0)
					{
						aliasClassInfo = ReflectClass(aliasedType);
					}
					break;

				case CXTypeKind.CXType_Enum:
					aliasEnumInfo = Registry.GetEnum(aliasedName);
					break;
			}

			aliasInfo = new AliasInfo
			{
				Type = ReflectType(cursor.Type),
				AliasType = ReflectType(aliasedType),
				AliasClass = aliasClassInfo,
				AliasEnum = aliasEnumInfo,
				Metadata = metadata
			};
			Registry.AddAlias(aliasInfo);
		}

		return aliasInfo;
	}

	/// <summary>
	/// Reflect a global function (i.e. non class method).
	/// </summary>
	/// <param name="cursor"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	private FunctionInfo ReflectFunction(CXCursor cursor, MetadataInfo metadata)
	{
		ClangUtils.GetNameInfo(cursor, out var qualifedName, out var name, out var @namespace);

		var functionInfo = Registry.GetFunction(qualifedName);
		if (functionInfo == null)
		{
			ReflectFunctionCommon(cursor, out var returnType, out var parameterTypes);

			functionInfo = new FunctionInfo
			{
				Metadata = metadata,
				Name = name,
				Namespace = @namespace,
				ReturnType = returnType,
				ArgumentTypes = parameterTypes
			};

			Registry.AddFunction(functionInfo);
		}

		return functionInfo;
	}
}
