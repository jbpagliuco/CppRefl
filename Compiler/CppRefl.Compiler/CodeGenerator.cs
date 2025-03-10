﻿using CppRefl.Compiler.CodeGenerators.Optional;
using CppRefl.Compiler.CodeGenerators.Runtime;
using CppRefl.Compiler.CodeGenerators.STL;
using CppRefl.Compiler.Reflection;
using System.Reflection;

namespace CppRefl.Compiler
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
		public required DirectoryInfo ModuleDirectory { get; init; }

		/// <summary>
		/// Output directory that will contain the generated files.
		/// </summary>
		public required DirectoryInfo OutputDirectory { get; init; }

		// Output file extension prefix.
		public string OutputExtensionPrefix { get; init; } = CodeGenerator.DefaultGeneratedExtensionPrefix;
		public string GeneratedHeaderExtension => CodeGenerator.GeneratedHeaderExtension(OutputExtensionPrefix);
		public string GeneratedSourceExtension => CodeGenerator.GeneratedSourceExtension(OutputExtensionPrefix);

		// List of assemblies that contain generator extensions.
		public List<Assembly> ExtensionAssemblies { get; init; } = new();
	}

	/// <summary>
	/// Parameters for generating per-file reflection code.
	/// </summary>
	public class CodeGeneratorFileParams : CodeGeneratorParamsCommon
	{
		/// <summary>
		/// Input source filename.
		/// </summary>
		public required FileInfo InputFilename { get; init; }
	}

	/// <summary>
	/// Parameters for generating per-registry reflection code.
	/// </summary>
	public class CodeGeneratorModuleParams : CodeGeneratorParamsCommon
	{
		/// <summary>
		/// Module name.
		/// </summary>
		public required string ModuleName { get; init; }
	}

	/// <summary>
	/// A file that is generated by this code generator.
	/// </summary>
	public sealed class GeneratedOutputFile
	{
		/// <summary>
		/// The filename containing our results.
		/// </summary>
		public required FileInfo Filename { get; init; }

		/// <summary>
		/// The stream containing our file contents.
		/// </summary>
		public required Stream Stream { get; init; }

		/// <summary>
		/// Write data to our stream.
		/// </summary>
		/// <param name="data"></param>
		public void Write(string data)
		{
			using var writer = new StreamWriter(Stream);
			writer.Write(data);
		}
	}

	public sealed class FileCodeGeneratorResult
	{
		/// <summary>
		/// The header file contents.
		/// </summary>
		public required GeneratedOutputFile Header { get; init; }

		/// <summary>
		/// The source file contents.
		/// </summary>
		public required GeneratedOutputFile Source { get; init; }

		/// <summary>
		/// The registry file contents.
		/// </summary>
		public required GeneratedOutputFile Registry { get; init; }
	}

	public sealed class ModuleCodeGeneratorResult
	{
		/// <summary>
		/// The module header file contents.
		/// </summary>
		public required GeneratedOutputFile Header { get; init; }

		/// <summary>
		/// The module source file contents.
		/// </summary>
		public required GeneratedOutputFile Source { get; init; }

		/// <summary>
		/// The module registry contents.
		/// </summary>
		public required GeneratedOutputFile Registry { get; init; }
	}

	/// <summary>
	/// Writes code for a program.
	/// </summary>
	public class CodeGenerator
	{
		/// <summary>
		/// Extension for a generated header file.
		/// </summary>
		public const string DefaultGeneratedExtensionPrefix = ".reflgen";

		/// <summary>
		/// Extension for a generated header file.
		/// </summary>
		public static string GeneratedHeaderExtension(string prefix = DefaultGeneratedExtensionPrefix) => $"{prefix}.h";

		/// <summary>
		/// Extension for a generated source file.
		/// </summary>
		public static string GeneratedSourceExtension(string prefix = DefaultGeneratedExtensionPrefix) => $"{prefix}.cpp";

		public CodeGenerator()
		{
			// Essential
			AddFileCodeGenerator<ClassReflectionGenerator>();
			AddFileCodeGenerator<EnumReflectionGenerator>();
			AddFileCodeGenerator<FunctionReflectionGenerator>();
			AddModuleCodeGenerator<ModuleReflectionGenerator>();

			// Optional
			AddFileCodeGenerator<ClassSuperGenerator>();
			AddFileCodeGenerator<ClassMemberTypeGetters>();
			AddFileCodeGenerator<ClassStaticTypeGetters>();
			AddFileCodeGenerator<VectorDynamicArrayGenerator>();
		}

		public List<IFileCodeGenerator> FileGenerators { get; } = new();
		public List<IModuleCodeGenerator> ModuleGenerators { get; } = new();

		/// <summary>
		/// Returns the generated output file from a source file.
		/// </summary>
		/// <returns></returns>
		public static FileInfo SourceToGenerated(FileInfo sourceFile, DirectoryInfo moduleDirectory, DirectoryInfo outputDirectory, string extension)
		{
			var relative = Path.GetRelativePath(moduleDirectory.FullName, sourceFile.DirectoryName!);
			var filename = $"{Path.GetFileNameWithoutExtension(sourceFile.FullName)}{extension}";
			return new(Path.Combine(outputDirectory.FullName, relative, filename));
		}

		/// <summary>
		/// Returns the source file that generated an output file.
		/// </summary>
		/// <returns></returns>
		public static FileInfo GeneratedToSource(FileInfo outputFile, DirectoryInfo moduleDirectory, DirectoryInfo outputDirectory, string extension)
		{
			var relative = Path.GetRelativePath(outputDirectory.FullName, outputFile.DirectoryName!);
			var filename = $"{Path.GetFileNameWithoutExtension(Path.GetFileNameWithoutExtension(outputFile.FullName))}{extension}";
			return new(Path.Combine(moduleDirectory.FullName, relative, filename));
		}

		/// <summary>
		/// Add a code generator.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		public T AddFileCodeGenerator<T>() where T : IFileCodeGenerator, new()
		{
			T generator = new();
			FileGenerators.Add(generator);
			return generator;
		}

		/// <summary>
		/// Add a code generator.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		public T AddModuleCodeGenerator<T>() where T : IModuleCodeGenerator, new()
		{
			T generator = new();
			ModuleGenerators.Add(generator);
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
				else if (type.GetInterface(nameof(IModuleCodeGenerator)) != null)
				{
					if (Activator.CreateInstance(type) is IModuleCodeGenerator generator)
					{
						ModuleGenerators.Add(generator);
					}
				}
			}
		}

		/// <summary>
		/// Adds a code generator extension from an assembly.
		/// </summary>
		/// <param name="assemblies"></param>
		public void LoadCodeGenerators(IList<Assembly> assemblies)
		{
			foreach (Assembly assembly in assemblies)
			{
				LoadCodeGenerators(assembly);
			}
		}

		/// <summary>
		/// Generate code pertaining to a single file.
		/// </summary>
		/// <param name="params"></param>
		public FileCodeGeneratorResult GenerateFileCode(CodeGeneratorFileParams @params, Func<FileInfo, Stream>? streamCreator = null)
		{
			streamCreator ??= fileInfo => new FileStream(fileInfo.FullName, FileMode.Create, FileAccess.ReadWrite);

			FileCodeGeneratorContext context = new(@params, @params.Registry.GetObjectsInFile(@params.InputFilename));

			LoadCodeGenerators(@params.ExtensionAssemblies);
			foreach (var generator in FileGenerators)
			{
				generator.Execute(context);
			}

			var headerFilename = SourceToGenerated(@params.InputFilename, @params.ModuleDirectory, @params.OutputDirectory, @params.GeneratedHeaderExtension);
			var sourceFilename = SourceToGenerated(@params.InputFilename, @params.ModuleDirectory, @params.OutputDirectory, @params.GeneratedSourceExtension);
			var registryFilename = SourceToGenerated(@params.InputFilename, @params.ModuleDirectory, @params.OutputDirectory, Registry.RegistryExtension);
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
				},
				Registry = new()
				{
					Filename = registryFilename,
					Stream = streamCreator(registryFilename)
				}
			};

			result.Header.Write(context.DumpHeader());
			result.Source.Write(context.DumpSource());
			result.Registry.Write(@params.Registry.ToJson());

			return result;
		}

		/// <summary>
		/// Generate code pertaining to the module registry.
		/// </summary>
		/// <param name="params"></param>
		public ModuleCodeGeneratorResult GenerateModuleCode(CodeGeneratorModuleParams @params, Func<FileInfo, Stream>? streamCreator = null)
		{
			streamCreator ??= fileInfo => new FileStream(fileInfo.FullName, FileMode.Create, FileAccess.ReadWrite);

			ModuleCodeGeneratorContext context = new()
			{
				Parameters = @params
			};

			LoadCodeGenerators(@params.ExtensionAssemblies);
			foreach (var generator in ModuleGenerators)
			{
				generator.Execute(context);
			}

			FileInfo headerFilename = new(Path.Combine(@params.OutputDirectory.FullName, $"{@params.ModuleName}{@params.GeneratedHeaderExtension}"));
			FileInfo sourceFilename = new(Path.Combine(@params.OutputDirectory.FullName, $"{@params.ModuleName}{@params.GeneratedSourceExtension}"));
			FileInfo registryFilename = new(Path.Combine(@params.OutputDirectory.FullName, $"{@params.ModuleName}{Registry.RegistryExtension}"));
			ModuleCodeGeneratorResult result = new()
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
				},
				Registry = new()
				{
					Filename = registryFilename,
					Stream = streamCreator(registryFilename)
				}
			};

			result.Header.Write(context.DumpHeader());
			result.Source.Write(context.DumpSource());
			result.Registry.Write(@params.Registry.ToJson());

			return result;
		}
	}
}
