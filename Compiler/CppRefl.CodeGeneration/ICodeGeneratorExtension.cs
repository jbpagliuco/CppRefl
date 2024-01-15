using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration
{
	public interface ICodeGeneratorExtension
	{
		/// <summary>
		/// Writes code at the top of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="objects"></param>
		/// <param name="registry"></param>
		public void WriteHeaderTop(CppWriter writer, FileObjects objects, Registry registry)
		{
		}

		/// <summary>
		/// Writes code at the bottom of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="objects"></param>
		/// <param name="registry"></param>
		public void WriteHeaderBottom(CppWriter writer, FileObjects objects, Registry registry)
		{
		}

		/// <summary>
		/// Writes code at the top of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="objects"></param>
		/// <param name="registry"></param>
		public void WriteSourceTop(CppWriter writer, FileObjects objects, Registry registry)
		{
		}

		/// <summary>
		/// Writes code at the bottom of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="objects"></param>
		/// <param name="registry"></param>
		public void WriteSourceBottom(CppWriter writer, FileObjects objects, Registry registry)
		{
		}

		/// <summary>
		/// Generate code that belongs to the body of an existing class.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassDeclaration(CppWriter writer, ClassInfo classInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate per-class code in a header file (this is not pasted into the class body).
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassHeader(CppWriter writer, ClassInfo classInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate per-class code in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassSource(CppWriter writer, ClassInfo classInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for an enum in a header file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="enumInfo"></param>
		/// <param name="registry"></param>
		public void WriteEnumHeader(CppWriter writer, EnumInfo enumInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for an enum in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="enumInfo"></param>
		/// <param name="registry"></param>
		public void WriteEnumSource(CppWriter writer, EnumInfo enumInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for an alias in a header file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="aliasInfo"></param>
		/// <param name="registry"></param>
		public void WriteAliasHeader(CppWriter writer, AliasInfo aliasInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for an alias in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="aliasInfo"></param>
		/// <param name="registry"></param>
		public void WriteAliasSource(CppWriter writer, AliasInfo aliasInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for a function in a header file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="functionInfo"></param>
		/// <param name="registry"></param>
		public void WriteFunctionHeader(CppWriter writer, FunctionInfo functionInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for a function in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="functionInfo"></param>
		/// <param name="registry"></param>
		public void WriteFunctionSource(CppWriter writer, FunctionInfo functionInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate header code pertaining to the module registry.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="registry"></param>
		public void WriteRegistryHeader(CppWriter writer, Registry registry, CodeGeneratorRegistryParams @params)
		{
		}

		/// <summary>
		/// Generate source code pertaining to the module registry.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="registry"></param>
		public void WriteRegistrySource(CppWriter writer, Registry registry, CodeGeneratorRegistryParams @params)
		{
		}

		/// <summary>
		/// Generate source code pertaining to the module registry.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="registry"></param>
		public void WriteRegistryInitializer(CppWriter writer, Registry registry, CodeGeneratorRegistryParams @params)
		{
		}
	}
}
