using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using CppRefl.CodeGeneration.CodeGenerators;
using CppRefl.CodeGeneration.CodeGenerators.Optional;
using CppRefl.CodeGeneration.CodeGenerators.Runtime;
using CppRefl.CodeGeneration.CodeGenerators.STL;
using CppRefl.CodeGeneration.CodeWriters;
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

	public sealed class FileGenerationResult
	{
		/// <summary>
		/// The filename containing our results.
		/// </summary>
		public required string Filename { get; init; }

		/// <summary>
		/// The stream containing our file contents.
		/// </summary>
		public required Stream Stream { get; init; }
	}

	public sealed class FileCodeGeneratorResult
	{
		/// <summary>
		/// The header file contents.
		/// </summary>
		public required FileGenerationResult Header { get; init; }

		/// <summary>
		/// The source file contents.
		/// </summary>
		public required FileGenerationResult Source { get; init; }
	}

	/// <summary>
	/// Writes code for a program.
	/// </summary>
	public class CodeGenerator
	{
		public const string FileHeaderExt = ".reflgen.h";
		public const string FileSourceExt = ".reflgen.cpp";

		public const string ModuleHeaderExt = ".reflmodule.h";
		public const string ModuleSourceExt = ".reflmodule.cpp";

		public List<IFileCodeGenerator> FileGenerators { get; } = new();

		public static string GetOutputFilename(string file, string moduleDirectory, string outputDirectory, string extension) => Path.Combine(outputDirectory, Path.GetRelativePath(moduleDirectory, Path.GetDirectoryName(file)!),
				$"{Path.GetFileNameWithoutExtension(file)}{extension}");

		//private string RegistryInitializerSignature(string moduleName) => $"void Registry{moduleName}Reflection()";

		//public CodeGenerator()
		//{
		//	// Essential
		//	LoadCodeGenerator<ClassReflectionGenerator>();
		//	LoadCodeGenerator<EnumReflectionGenerator>();
		//	LoadCodeGenerator<FunctionStaticGenerator>();
		//	LoadCodeGenerator<VectorDynamicArrayGenerator>();
		//	LoadCodeGenerator<RegistryGenerator>();

		//	// Optional
		//	// LoadCodeGenerator<ClassSuperGenerator>();
		//	LoadCodeGenerator<ClassMemberTypeGetters>();
		//	LoadCodeGenerator<ClassStaticTypeGetters>();
		//}

		/// <summary>
		/// Add a code generator.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		public T AddCodeGenerator<T>() where T : IFileCodeGenerator, new()
		{
			T generator = new();
			FileGenerators.Add(generator);
			return generator;
		}

		/// <summary>
		/// Adds a code generator extension from an assembly.
		/// </summary>
		/// <param name="assembly"></param>
		public void LoadCodeGenerators(Assembly assembly)
		{
			foreach (Type type in assembly.GetExportedTypes())
			{
				if (type.GetInterface(nameof(IFileCodeGenerator)) != null)
				{
					if (Activator.CreateInstance(type) is IFileCodeGenerator generator)
					{
						FileGenerators.Add(generator);
					}
				}
			}
		}
		
		/// <summary>
		/// Generate code pertaining to a single file.
		/// </summary>
		/// <param name="params"></param>
		public FileCodeGeneratorResult GenerateFileCode(CodeGeneratorFileParams @params, Func<string, Stream>? streamCreator = null)
		{
			streamCreator ??= filename => new FileStream(filename, FileMode.Create, FileAccess.Write);

			IEnumerable<T> FindFileObjects<T>(IEnumerable<T> items) where T : ObjectInfo
			{
				return items.Where(x =>
					x.Metadata.IsReflected && 
					x.Metadata.SourceLocation.Filepath == @params.InputFilename);
			}

			// Find all reflected classes in our input file.
			FileObjects objects = new()
			{
				Classes = FindFileObjects(@params.Registry.GetClasses()),
				Enums = FindFileObjects(@params.Registry.GetEnums()),
				Aliases = FindFileObjects(@params.Registry.GetAliases()),
				Functions = FindFileObjects(@params.Registry.GetFunctions())
			};

			FileCodeGeneratorContext context = new()
			{
				Registry = @params.Registry,
				Objects = objects,
				Parameters = @params
			};

			foreach (var generator in FileGenerators)
			{
				generator.Execute(context);
			}

			var headerFilename = GetOutputFilename(@params.InputFilename, @params.ModuleDirectory, @params.OutputDirectory, FileHeaderExt);
			var sourceFilename = GetOutputFilename(@params.InputFilename, @params.ModuleDirectory, @params.OutputDirectory, FileSourceExt);
			FileCodeGeneratorResult result = new()
			{
				Header = new()
				{
					Filename = headerFilename,
					Stream = streamCreator(headerFilename)
				},
				Source = new()
				{
					Filename = sourceFilename,
					Stream = streamCreator(sourceFilename)
				}
			};

			using (var writer = new StreamWriter(result.Header.Stream))
			{
				writer.Write(context.DumpHeader());
			}

			using (var writer = new StreamWriter(result.Source.Stream))
			{
				writer.Write(context.DumpSource());
			}

			return result;
		}

		///// <summary>
		///// Generate code pertaining to the module registry.
		///// </summary>
		///// <param name="params"></param>
		//public void GenerateRegistryCode(CodeGeneratorRegistryParams @params, CodeGeneratorContext context)
		//{
		//	LoadCodeGenerators(@params.ExternalGeneratorDlls);

		//	string headerFilename = Path.Combine(@params.OutputDirectory, $"{@params.ModuleName}{RegistryHeaderExt}");
		//	string sourceFilename = Path.Combine(@params.OutputDirectory, $"{@params.ModuleName}{RegistrySourceExt}");

		//	// Header
		//	{
		//		using var writer = new CppWriter(headerFilename);

		//		// Initializer prototype.
		//		writer.WriteLine("#pragma once\n");
		//		writer.WriteLine("// Registers the reflection classes for this module.");
		//		writer.WriteLine($"{RegistryInitializerSignature(@params.ModuleName)};");

		//		InvokeGenerators(gen => gen.WriteRegistryHeader(context));
		//	}

		//	// Source
		//	{
		//		using var writer = new CppWriter(sourceFilename);

		//		writer.IncludeHeader(Path.GetFileName(headerFilename));
		//		writer.WriteLine();

		//		writer.IncludeHeader("CppReflStatics.h");
		//		writer.WriteLine();

		//		InvokeGenerators(gen => gen.WriteRegistrySource(context, @params));

		//		// Initializer definition.
		//		using (writer.WithFunction(RegistryInitializerSignature(@params.ModuleName)))
		//		{
		//			InvokeGenerators(gen => gen.WriteRegistryInitializer(context, @params));
		//		}
		//	}
		//}
	}
}
