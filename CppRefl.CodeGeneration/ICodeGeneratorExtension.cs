﻿using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration
{
	public record FileObjects
	{
		/// <summary>
		/// List of reflected classes in the given file.
		/// </summary>
		public IEnumerable<ClassInfo> Classes { get; init; } = Enumerable.Empty<ClassInfo>();

		/// <summary>
		/// List of reflected enums in the given file.
		/// </summary>
		public IEnumerable<EnumInfo> Enums { get; init; } = Enumerable.Empty<EnumInfo>();

		/// <summary>
		/// List of reflected aliases in the given file.
		/// </summary>
		public IEnumerable<AliasInfo> Aliases { get; init; } = Enumerable.Empty<AliasInfo>();

		/// <summary>
		/// List of reflected functions in the given file.
		/// </summary>
		public IEnumerable<FunctionInfo> Functions { get; init; } = Enumerable.Empty<FunctionInfo>();

		public bool Any() => Classes.Any() || Enums.Any() || Aliases.Any() || Functions.Any();
	}


	public interface ICodeGeneratorExtension
	{
		/// <summary>
		/// Writes code at the top of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="objects"></param>
		/// <param name="registry"></param>
		public void WriteHeaderTop(CodeWriter writer, FileObjects objects, Registry registry)
		{
		}

		/// <summary>
		/// Writes code at the bottom of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="objects"></param>
		/// <param name="registry"></param>
		public void WriteHeaderBottom(CodeWriter writer, FileObjects objects, Registry registry)
		{
		}

		/// <summary>
		/// Writes code at the top of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="objects"></param>
		/// <param name="registry"></param>
		public void WriteSourceTop(CodeWriter writer, FileObjects objects, Registry registry)
		{
		}

		/// <summary>
		/// Writes code at the bottom of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="objects"></param>
		/// <param name="registry"></param>
		public void WriteSourceBottom(CodeWriter writer, FileObjects objects, Registry registry)
		{
		}

		/// <summary>
		/// Generate code that belongs to the body of an existing class.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassDeclaration(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate per-class code in a header file (this is not pasted into the class body).
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassHeader(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate per-class code in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassSource(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for an enum in a header file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="enumInfo"></param>
		/// <param name="registry"></param>
		public void WriteEnumHeader(CodeWriter writer, EnumInfo enumInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for an enum in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="enumInfo"></param>
		/// <param name="registry"></param>
		public void WriteEnumSource(CodeWriter writer, EnumInfo enumInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for an alias in a header file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="aliasInfo"></param>
		/// <param name="registry"></param>
		public void WriteAliasHeader(CodeWriter writer, AliasInfo aliasInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for an alias in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="aliasInfo"></param>
		/// <param name="registry"></param>
		public void WriteAliasSource(CodeWriter writer, AliasInfo aliasInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for a function in a header file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="functionInfo"></param>
		/// <param name="registry"></param>
		public void WriteFunctionHeader(CodeWriter writer, FunctionInfo functionInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate code for a function in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="functionInfo"></param>
		/// <param name="registry"></param>
		public void WriteFunctionSource(CodeWriter writer, FunctionInfo functionInfo, Registry registry)
		{
		}

		/// <summary>
		/// Generate header code pertaining to the module registry.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="registry"></param>
		public void WriteRegistryHeader(CodeWriter writer, Registry registry, CodeGeneratorRegistryParams @params)
		{
		}

		/// <summary>
		/// Generate source code pertaining to the module registry.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="registry"></param>
		public void WriteRegistrySource(CodeWriter writer, Registry registry, CodeGeneratorRegistryParams @params)
		{
		}

		/// <summary>
		/// Generate source code pertaining to the module registry.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="registry"></param>
		public void WriteRegistryInitializer(CodeWriter writer, Registry registry, CodeGeneratorRegistryParams @params)
		{
		}
	}
}
