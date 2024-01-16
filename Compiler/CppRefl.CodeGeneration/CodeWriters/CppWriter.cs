using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeWriters
{
    public class CppWriter : CodeWriter
    {
	    public CodeWriterCodeBlock WithClass(string className) => WithCodeBlock($"class {className}", "{", "};");
	    public CodeWriterCodeBlock WithStruct(string structName) => WithCodeBlock($"struct {structName}", "{", "};");
	    public CodeWriterCodeBlock WithNamespace(string namespaceName) => WithCodeBlock($"namespace {namespaceName}", "{", "}");
	    public CodeWriterCodeBlock WithFunction(string signature) => WithCodeBlock(signature, "{", "}");

	    /// <summary>
	    /// Write an #include line.
	    /// </summary>
	    /// <param name="header"></param>
	    public void IncludeHeader(string header)
	    {
		    WriteLine($"#include \"{header.Replace("\\", "/")}\"");
	    }

	    /// <summary>
	    /// Writes a section of code under a "public" block.
	    /// </summary>
	    /// <returns></returns>
	    public CodeWriterIndent WithPublic()
	    {
		    WriteLine("public:");
		    return WithIndent();
	    }

	    /// <summary>
	    /// Writes a section of code under a "protected" block.
	    /// </summary>
	    /// <returns></returns>
	    public CodeWriterIndent WithProtected()
	    {
		    WriteLine("protected:");
		    return WithIndent();
	    }

	    /// <summary>
	    /// Writes a section of code under a "private" block.
	    /// </summary>
	    /// <returns></returns>
	    public CodeWriterIndent WithPrivate()
	    {
		    WriteLine("private:");
		    return WithIndent();
	    }

	    /// <summary>
		/// Forward declare a class. NB: This MUST be called from the global namespace.
		/// </summary>
		/// <param name="classInfo"></param>
	    public void ForwardDeclare(ClassInfo classInfo)
	    {
			WriteLine(classInfo.ForwardDeclaration);
		}

	    /// <summary>
		/// Forward declare an enum. NB: This MUST be called from the global namespace.
		/// </summary>
		/// <param name="enumInfo"></param>
		public void ForwardDeclare(EnumInfo enumInfo)
		{
			var namespaceDec = enumInfo.Type.IsInGlobalNamespace() ? null : WithNamespace(enumInfo.Type.Namespace);
			WriteLine($"enum class {enumInfo.Type.Name};");
			namespaceDec?.Dispose();
		}

	    /// <summary>
		/// Forward declare a function. NB: This MUST be called from the global namespace.
		/// </summary>
		/// <param name="functionInfo"></param>
		public void ForwardDeclare(FunctionInfo functionInfo)
		{
			var namespaceDec = functionInfo.IsInGlobalNamespace() ? null : WithNamespace(functionInfo.Namespace);
			WriteLine($"{functionInfo.Signature};");
			namespaceDec?.Dispose();
		}
    }
}
