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
		/// The source program's reflection registry.
		/// </summary>
		public required Registry Registry { get; init; }

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
			StringBuilder sb = new(base.DumpHeader());

			foreach (var (classInfo, writer) in ClassDeclarationWriters)
			{
				string generatedBodyMacroName = $"{CppDefines.InternalReflectionMacroPrefix}_{classInfo.Metadata.SourceLocation.FilenameNoExt}{classInfo.GeneratedBodyLine}";
				sb.AppendLine($"""
			                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                  // {classInfo.Type.QualifiedName} class declaration
			                  
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

			return sb.ToString();
		}
	}
}
