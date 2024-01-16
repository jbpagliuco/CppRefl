using System.Text;
using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration
{
	/// <summary>
	/// Code generation diagnostic level.
	/// </summary>
	public enum DiagnosticLevel
	{
		Warning,
		Error
	}

	/// <summary>
	/// The context that a code generator runs in.
	/// </summary>
	public class CodeGeneratorContext
	{
		/// <summary>
		/// Writer for our header file.
		/// </summary>
		private CppWriter HeaderWriter { get; } = new();

		/// <summary>
		/// Writer for our source file.
		/// </summary>
		private CppWriter SourceWriter { get; } = new();

		/// <summary>
		/// Adds a diagnostic message.
		/// </summary>
		/// <param name="level"></param>
		/// <param name="message"></param>
		public void AddDiagnostic(DiagnosticLevel level, string message)
		{
			switch (level)
			{
				case DiagnosticLevel.Warning:
					Console.Error.WriteLine(message);
					break;

				case DiagnosticLevel.Error:
					// TODO: Should build up all errors and then throw at the end.
					throw new Exception(message);
			}
		}

		/// <summary>
		/// Write contents to our header file.
		/// </summary>
		/// <param name="action"></param>
		public void WriteHeader(Action<CppWriter> action) => action(HeaderWriter);

		/// <summary>
		/// Write contents to our source file.
		/// </summary>
		/// <param name="action"></param>
		public void WriteSource(Action<CppWriter> action) => action(SourceWriter);

		/// <summary>
		/// Dump the contents of our header file.
		/// </summary>
		/// <returns></returns>
		public virtual string DumpHeader() => HeaderWriter.ToString();

		/// <summary>
		/// Dump the countents of our source file.
		/// </summary>
		/// <returns></returns>
		public virtual string DumpSource() => SourceWriter.ToString();
	}

	/// <summary>
	/// The context that a file code generator runs in.
	/// </summary>
	public class FileCodeGeneratorContext : CodeGeneratorContext
	{
		/// <summary>
		/// The original code generator parameters.
		/// </summary>
		public required CodeGeneratorFileParams Parameters { get; init; }

		/// <summary>
		/// The objects declared in this file.
		/// </summary>
		public required FileObjects Objects { get; init; }

		private IDictionary<ClassInfo, CppWriter> ClassDeclarationWriters { get; } = new Dictionary<ClassInfo, CppWriter>();

		/// <summary>
		/// Invoke a writer action.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="dict"></param>
		/// <param name="key"></param>
		/// <param name="action"></param>
		private void InvokeWriterAction<T>(IDictionary<T, CppWriter> dict, T key, Action<CppWriter> action)
		{
			if (!dict.TryGetValue(key, out var writer))
			{
				writer = new CppWriter();
				dict.Add(key, writer);
			}

			action(writer);
		}

		/// <summary>
		/// Writes code to the body of a class declaration.
		/// </summary>
		/// <param name="classInfo"></param>
		/// <param name="action"></param>
		public void WriteClassDeclaration(ClassInfo classInfo, Action<CppWriter> action)
		{
			InvokeWriterAction(ClassDeclarationWriters, classInfo, writer =>
			{
				// Postfix everything with a backslash since this code goes into a macro.
				using (writer.WithPostfix("\\"))
				{
					using (writer.WithIndent(2))
					{
						action(writer);
					}
				}
			});
		}

		public override string DumpHeader()
		{
			StringBuilder sb = new();

			// Write the include guard at the top, as well as the file id which makes the GENERATED_REFLECTION_CODE macros work.
			string fileId = Path.GetFileNameWithoutExtension(Parameters.InputFilename.Name);
			string headerGuard = $"{fileId}_REFLGEN_H";
			sb.AppendLine($"""
			               #undef {CppDefines.FileId}
			               #define {CppDefines.FileId} {fileId}
			               
			               #if defined({headerGuard})
			                   #error Including {fileId}.reflgen.h multiple times! Use `#pragma once` in {fileId}.h.
			               #endif
			               
			               #define {headerGuard}
			               
			               #include "Private/CppReflGeneratedCodeMacros.h"
			               """);

			// Write the base header data.
			sb.AppendLine(base.DumpHeader());

			// Write all of the class bodies.
			foreach (var (classInfo, writer) in ClassDeclarationWriters)
			{
				if (classInfo.GeneratedBodyLine != null)
				{
					string generatedBodyMacroName =
						$"{CppDefines.InternalReflectionMacroPrefix}_{classInfo.Metadata.SourceLocation.FilenameNoExt}{classInfo.GeneratedBodyLine}";
					sb.AppendLine($"""
					               ////////////////////////////////////////////////////////////////////////////////////////////////////////
					               ////////////////////////////////////////////////////////////////////////////////////////////////////////
					               // {classInfo.Type.QualifiedName()} class declaration

					               #if !{CppDefines.BuildReflection}
					               // Macro to be added inside the definition of a reflected class.
					               	#define {generatedBodyMacroName}()\
					               {writer}

					               #else
					               	// Define empty macro when building reflection.
					               	#define {generatedBodyMacroName}()
					               #endif

					               ////////////////////////////////////////////////////////////////////////////////////////////////////////
					               ////////////////////////////////////////////////////////////////////////////////////////////////////////
					               """);
				}
			}

			return sb.ToString();
		}

		public override string DumpSource()
		{
			StringBuilder sb = new();

			// Include the original source file.
			string originalFilenameRelative = Path.GetRelativePath(Parameters.ModuleDirectory.FullName, Parameters.InputFilename.FullName).Replace("\\", "/");
			sb.AppendLine($"""#include "{originalFilenameRelative}" """);

			// Write the base data.
			sb.AppendLine(base.DumpSource());

			return sb.ToString();
		}
	}

	/// <summary>
	/// The context that a module code generator runs in.
	/// </summary>
	public class ModuleCodeGeneratorContext : CodeGeneratorContext
	{
		/// <summary>
		/// The original code generator parameters.
		/// </summary>
		public required CodeGeneratorModuleParams Parameters { get; init; }

		/// <summary>
		/// Writer for our initializer function.
		/// </summary>
		private CppWriter InitializerWriter { get; } = new();

		private string InitializerSignature => $"void Register{Parameters.ModuleName}Reflection()";

		/// <summary>
		/// Writes code to the body of module initializer function.
		/// </summary>
		/// <param name="action"></param>
		public void WriteInitializer(Action<CppWriter> action)
		{
			// Indent this code since it's contained within a function.
			using (InitializerWriter.WithIndent())
			{
				action(InitializerWriter);
			}
		}

		public override string DumpHeader()
		{
			StringBuilder sb = new();

			sb.AppendLine($"""
			               #pragma once

			               // Register the reflection runtime information for this module.
			               {InitializerSignature};
			               
			               {base.DumpHeader()}
			               """);
			
			return sb.ToString();
		}

		public override string DumpSource()
		{
			StringBuilder sb = new();
			
			sb.AppendLine($$"""
			               #include "{{Parameters.ModuleName}}{{CodeGenerator.GeneratedHeaderExtension}}"

			               #include "CppReflStatics.h"

			               {{base.DumpSource()}}
			               
			               {{InitializerSignature}}
			               {
			               {{InitializerWriter}}
			               }
			               """);

			return sb.ToString();
		}
	}
}
