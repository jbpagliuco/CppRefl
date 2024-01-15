using System.Text;

namespace CppRefl.CodeGeneration.CodeWriters
{
    public class CppWriter : CodeWriter
	{
		public CppWriter(string filename) : base(filename)
		{
		}

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
	}
}
