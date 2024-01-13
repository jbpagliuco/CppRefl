using System.Reflection;
using CppRefl.CodeGeneration.CodeGenerators;
using CppRefl.CodeGeneration.CodeGenerators.STL;
using CppRefl.CodeGeneration.Reflection;
using TypeInfo = CppRefl.CodeGeneration.Reflection.TypeInfo;

namespace CppRefl.CodeGeneration
{
	public class CodeGeneratorParamsCommon
	{
		/// <summary>
		/// Reflection registry.
		/// </summary>
		public required Registry Registry { get; init; }

		/// <summary>
		/// Module directory.
		/// </summary>
		private readonly string _moduleDirectory = string.Empty;
		public required string ModuleDirectory { get => _moduleDirectory; init => _moduleDirectory = Path.GetFullPath(value); }

		/// <summary>
		/// Output directory that will contain the generated files.
		/// </summary>
		private readonly string _outputDirectory = string.Empty;
		public required string OutputDirectory { get => _outputDirectory; init => _outputDirectory = Path.GetFullPath(value); }

		/// <summary>
		/// List of external .DLLs to load.
		/// </summary>
		public IEnumerable<string> ExternalGeneratorDlls { get; init; } = Enumerable.Empty<string>();
	}

	/// <summary>
	/// Parameters for generating per-file reflection code.
	/// </summary>
	public class CodeGeneratorFileParams : CodeGeneratorParamsCommon
	{
		/// <summary>
		/// Input source filename.
		/// </summary>
		public required string InputFilename { get; init; }
	}

	/// <summary>
	/// Parameters for generating per-registry reflection code.
	/// </summary>
	public class CodeGeneratorRegistryParams : CodeGeneratorParamsCommon
	{
		/// <summary>
		/// Module name.
		/// </summary>
		public required string ModuleName { get; init; }
	}

	/// <summary>
	/// Writes code for a program.
	/// </summary>
	public class CodeGenerator
	{
		public const string FileHeaderExt = ".reflgen.h";
		public const string FileSourceExt = ".reflgen.cpp";

		public const string RegistryHeaderExt = ".reflregistry.h";
		public const string RegistrySourceExt = ".reflregistry.cpp";

		private List<ICodeGeneratorExtension> Generators { get; } = new();

		public static string GetOutputFilename(string file, string moduleDirectory, string outputDirectory, string extension) => Path.Combine(outputDirectory, Path.GetRelativePath(moduleDirectory, Path.GetDirectoryName(file)!),
				$"{Path.GetFileNameWithoutExtension(file)}{extension}");

		private string RegistryInitializerSignature(string moduleName) => $"void Register{moduleName}Reflection()";

		public CodeGenerator()
		{
			LoadCodeGenerator<ClassStaticGenerator>();
			LoadCodeGenerator<ClassSuperGenerator>();
			LoadCodeGenerator<EnumStaticGenerator>();
			LoadCodeGenerator<FunctionStaticGenerator>();
			LoadCodeGenerator<VectorDynamicArrayGenerator>();
			LoadCodeGenerator<RegistryGenerator>();
		}

		/// <summary>
		/// Add a code generator extension.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		public void LoadCodeGenerator<T>() where T : ICodeGeneratorExtension, new()
		{
			Generators.Add(new T());
		}

		/// <summary>
		/// Adds a code generator extension from a .DLL file.
		/// </summary>
		/// <param name="dllFilename"></param>
		public void LoadCodeGenerator(string dllFilename)
		{
			if (!File.Exists(dllFilename))
			{
				throw new FileNotFoundException($"Code generator extension not found: {dllFilename}");
			}

			var dll = Assembly.LoadFile(dllFilename);

			foreach (Type type in dll.GetExportedTypes())
			{
				if (type.GetInterface(nameof(ICodeGeneratorExtension)) != null)
				{
					if (Activator.CreateInstance(type) is ICodeGeneratorExtension generator)
					{
						Generators.Add(generator);
					}
				}
			}
		}

		public void LoadCodeGenerators(IEnumerable<string> dlls)
		{
			foreach (var dll in dlls)
			{
				LoadCodeGenerator(dll);
			}
		}

		/// <summary>
		/// Generate code pertaining to a single file.
		/// </summary>
		/// <param name="params"></param>
		public void GenerateFileCode(CodeGeneratorFileParams @params)
		{
			LoadCodeGenerators(@params.ExternalGeneratorDlls);

			// Find all reflected classes in our input file.
			FileObjects objects = new()
			{
				Classes = @params.Registry.GetClasses()
					.Where(x => x.Metadata.SourceLocation.Filepath == @params.InputFilename)
					.Where(x => x.Metadata.IsReflected),

				Enums = @params.Registry.GetEnums()
					.Where(x => x.Metadata.SourceLocation.Filepath == @params.InputFilename)
					.Where(x => x.Metadata.IsReflected),

				Aliases = @params.Registry.GetAliases()
					.Where(x => x.Metadata.SourceLocation.Filepath == @params.InputFilename)
					.Where(x => x.Metadata.IsReflected),

				Functions = @params.Registry.GetFunctions()
					.Where(x => x.Metadata.SourceLocation.Filepath == @params.InputFilename)
					.Where(x => x.Metadata.IsReflected)
			};
			
			GenerateHeader(objects, @params);
			GenerateSource(objects, @params);
		}

		/// <summary>
		/// Generate code pertaining to the module registry.
		/// </summary>
		/// <param name="params"></param>
		public void GenerateRegistryCode(CodeGeneratorRegistryParams @params)
		{
			LoadCodeGenerators(@params.ExternalGeneratorDlls);

			string headerFilename = Path.Combine(@params.OutputDirectory, $"{@params.ModuleName}{RegistryHeaderExt}");
			string sourceFilename = Path.Combine(@params.OutputDirectory, $"{@params.ModuleName}{RegistrySourceExt}");

			// Header
			{
				using var writer = new CodeWriter(headerFilename);

				// Initializer prototype.
				writer.WriteLine("#pragma once\n");
				writer.WriteLine("// Registers the reflection classes for this module.");
				writer.WriteLine($"{RegistryInitializerSignature(@params.ModuleName)};");

				InvokeGenerators(gen => gen.WriteRegistryHeader(writer, @params.Registry, @params));
			}

			// Source
			{
				using var writer = new CodeWriter(sourceFilename);

				writer.IncludeHeader(Path.GetFileName(headerFilename));
				writer.WriteLine();

				writer.IncludeHeader("CppReflStatics.h");
				writer.WriteLine();

				InvokeGenerators(gen => gen.WriteRegistrySource(writer, @params.Registry, @params));

				// Initializer definition.
				using (writer.WithFunction(RegistryInitializerSignature(@params.ModuleName)))
				{
					InvokeGenerators(gen => gen.WriteRegistryInitializer(writer, @params.Registry, @params));
				}
			}
		}
		
		/// <summary>
		/// Invokes all generators.
		/// </summary>
		/// <param name="action"></param>
		private void InvokeGenerators(Action<ICodeGeneratorExtension> action)
		{
			foreach (var generator in Generators)
			{
				action(generator);
			}
		}

		/// <summary>
		/// Write an object.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="name"></param>
		/// <param name="action"></param>
		private void WriteObject(CodeWriter writer, string name, Action<ICodeGeneratorExtension> action)
		{
			writer.WriteLine($"""

			                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                  // {name}

			                  """);

			InvokeGenerators(action);

			writer.WriteLine($"""

			                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                  """);
		}

		private void WriteObject(CodeWriter writer, TypeInfo typeInfo, Action<ICodeGeneratorExtension> action) => WriteObject(writer, typeInfo.QualifiedName, action);
		private void WriteObject(CodeWriter writer, FunctionInfo functionInfo, Action<ICodeGeneratorExtension> action) => WriteObject(writer, functionInfo.QualifiedName, action);

		/// <summary>
		/// Generate a header file for a reflected file.
		/// </summary>
		/// <param name="objects"></param>
		private void GenerateHeader(FileObjects objects, CodeGeneratorFileParams @params)
		{
			string outputFilename = GetOutputFilename(@params.InputFilename, @params.ModuleDirectory, @params.OutputDirectory, FileHeaderExt);
			using var writer = new CodeWriter(outputFilename);

			if (objects.Any())
			{
				// Write out the top of the file.
				GenerateHeaderTop(writer);

				// Let any extensions populate the top of the file.
				InvokeGenerators(gen => gen.WriteHeaderTop(writer, objects, @params.Registry));

				// Generate the body of classes (this is code that is pasted into the class definition).
				foreach (var classInfo in objects.Classes)
				{
					GenerateClassDeclaration(writer, classInfo, @params.Registry);
				}

				// Generate header info for enums.
				foreach (var enumInfo in objects.Enums)
				{
					WriteObject(writer, enumInfo.Type, gen => gen.WriteEnumHeader(writer, enumInfo, @params.Registry));
				}

				// Generate header info for aliases.
				foreach (var aliasInfo in objects.Aliases)
				{
					WriteObject(writer, aliasInfo.Type,
						gen => gen.WriteAliasHeader(writer, aliasInfo, @params.Registry));
				}

				// Generate header info for functions.
				foreach (var functionInfo in objects.Functions)
				{
					WriteObject(writer, functionInfo,
						gen => gen.WriteFunctionHeader(writer, functionInfo, @params.Registry));
				}

				// Write out the bottom of the class file.
				InvokeGenerators(gen => gen.WriteHeaderBottom(writer, objects, @params.Registry));
			}
		}

		/// <summary>
		/// Generate the top of a class file.
		/// </summary>
		/// <param name="writer"></param>
		private void GenerateHeaderTop(CodeWriter writer)
		{
			// Define the file id
			string fileId = Path.GetFileNameWithoutExtension(Path.GetFileNameWithoutExtension(writer.Filename));
			writer.WriteLine($"""
			                  // This must be defined before we include the private macros so GENERATED_REFLECTION_CODE() works correctly.
			                  #undef {CppDefines.FileId}
			                  #define {CppDefines.FileId} {fileId}

			                  """);

			// Define the header guard
			string headerGuard = $"{fileId}_REFLGEN_H";
			writer.WriteLine($"""
			                  // Header guard. Raise an error if this file is included multiple times.
			                  #if defined({headerGuard})
			                  #error Including {Path.GetFileName(writer.Filename)} multiple times! Use `#pragma once` in {fileId}.h.
			                  #endif
			                  #define {headerGuard}

			                  """);

			// Include the generated macro magic.
			writer.WriteLine("#include \"Private/CppReflGeneratedCodeMacros.h\"");
			writer.WriteLine();
		}

		/// <summary>
		/// Generate a class declaration.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		private void GenerateClassDeclaration(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
			if (classInfo.GeneratedBodyLine != null)
			{
				string generatedBodyMacroName =
					$"{CppDefines.InternalReflectionMacroPrefix}_{classInfo.Metadata.SourceLocation.FilenameNoExt}{classInfo.GeneratedBodyLine}";

				writer.WriteLine($"""

				                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
				                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
				                  // {classInfo.Type.QualifiedName}

				                  #if !defined({CppDefines.BuildReflection})
				                  // Macro to be added inside the definition of a reflected class.
				                  #define {generatedBodyMacroName}() \
				                  """);

				using (writer.WithPostfix("\\"))
				{
					using (writer.Indent())
					{
						InvokeGenerators(gen => { gen.WriteClassDeclaration(writer, classInfo, registry); });
					}
				}

				writer.WriteLine($"""

				                  #else // !defined({CppDefines.BuildReflection})

				                  // Define empty macro when building reflection.
				                  #define {generatedBodyMacroName}()

				                  #endif // !defined({CppDefines.BuildReflection})

				                  """);
			}

			InvokeGenerators(gen => gen.WriteClassHeader(writer, classInfo, registry));

			writer.WriteLine($"""

			                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                  """);
		}

		/// <summary>
		/// Generate a source file for a reflected file.
		/// </summary>
		/// <param name="objects"></param>
		private void GenerateSource(FileObjects objects, CodeGeneratorFileParams @params)
		{
			string outputFilename = GetOutputFilename(@params.InputFilename, @params.ModuleDirectory,
				@params.OutputDirectory, FileSourceExt);
			using var writer = new CodeWriter(outputFilename);

			if (objects.Any())
			{
				// Include the original reflected file.
				writer.WriteLine($"#include \"{Path.GetRelativePath(@params.ModuleDirectory, @params.InputFilename)}\"");

				// Let any extensions populate the top of the file.
				InvokeGenerators(gen => gen.WriteSourceTop(writer, objects, @params.Registry));

				// Generate the body of classes (this is code that is pasted into the class definition).
				foreach (var classInfo in objects.Classes)
				{
					WriteObject(writer, classInfo.Type, gen => gen.WriteClassSource(writer, classInfo, @params.Registry));
				}

				// Generate source info for enums.
				foreach (var enumInfo in objects.Enums)
				{
					WriteObject(writer, enumInfo.Type, gen => gen.WriteEnumSource(writer, enumInfo, @params.Registry));
				}

				// Generate source info for aliases.
				foreach (var aliasInfo in objects.Aliases)
				{
					WriteObject(writer, aliasInfo.Type, gen => gen.WriteAliasSource(writer, aliasInfo, @params.Registry));
				}

				// Generate source info for functions.
				foreach (var functionInfo in objects.Functions)
				{
					WriteObject(writer, functionInfo, gen => gen.WriteFunctionSource(writer, functionInfo, @params.Registry));
				}

				// Write out the bottom of the class file.
				InvokeGenerators(gen => gen.WriteSourceBottom(writer, objects, @params.Registry));
			}
		}
	}
}
