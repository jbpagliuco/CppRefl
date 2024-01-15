using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.Tests
{
	internal class FileCodeGeneratorContextTests
	{
		/// <summary>
		/// Creates a default code generator context.
		/// </summary>
		/// <returns></returns>
		private static FileCodeGeneratorContext CreateFileCodeGeneratorContext()
		{
			var root = Path.GetTempPath();
			return new()
			{
				Objects = new(),
				Parameters = new()
				{
					InputFilename = $@"{root}Source/File.h",
					Registry = new(),
					ModuleDirectory = $@"{root}Source",
					OutputDirectory = $@"{root}Generated"
				}
			};
		}

		private string ExpectedHeaderPrefix =>
			$"""
			 #undef {CppDefines.FileId}
			 #define {CppDefines.FileId} File

			 #if defined(File_REFLGEN_H)
			     #error Including {Path.GetTempPath()}Source/File.h multiple times! Use `#pragma once` in File.h.
			 #endif

			 #define File_REFLGEN_H

			 #include "Private/CppReflGeneratedCodeMacros.h"
			 """;

		private string ExpectedSourcePrefix => "#include \"File.h\" ";

		[Test]
		public void ShouldWriteClassDeclaration()
		{
			FileCodeGeneratorContext context = CreateFileCodeGeneratorContext();
			ClassInfo classInfo = CodeGeneratorTestHelpers.CreateClassInfo();
			context.WriteClassDeclaration(classInfo, writer =>
			{
				writer.WriteLine("Line #1");
				writer.WriteLine("Line #2");
			});

			string generatedBodyMacroName = $"{CppDefines.InternalReflectionMacroPrefix}_{classInfo.Metadata.SourceLocation.FilenameNoExt}{classInfo.GeneratedBodyLine}";
			Assert.That(context.DumpHeader().Trim(), Is.EqualTo($$"""
			                                                     {{ExpectedHeaderPrefix}}
			                                                     
			                                                     ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                                                     ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                                                     // {{classInfo.Type.QualifiedName}} class declaration

			                                                     #if !{{CppDefines.BuildReflection}}
			                                                     // Macro to be added inside the definition of a reflected class.
			                                                     	#define {{generatedBodyMacroName}}()\
			                                                     		Line #1\
			                                                     		Line #2\
			                                                     
			                                                     
			                                                     #else
			                                                     	// Define empty macro when building reflection.
			                                                     	#define {{generatedBodyMacroName}}()
			                                                     #endif

			                                                     ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                                                     ////////////////////////////////////////////////////////////////////////////////////////////////////////
			                                                     """));
			Assert.That(context.DumpSource().Trim(), Is.EqualTo(ExpectedSourcePrefix.Trim()));
		}

		[Test]
		public void ShouldWriteHeader()
		{
			FileCodeGeneratorContext context = CreateFileCodeGeneratorContext();
			context.WriteHeader(writer =>
			{
				writer.WriteLine("Header Line #1.");
				writer.WriteLine("Header Line #2.");
			});

			Assert.That(context.DumpHeader().Trim(), Is.EqualTo($"""
			                                                     {ExpectedHeaderPrefix}
			                                                     Header Line #1.
			                                                     Header Line #2.
			                                                     """));
			Assert.That(context.DumpSource().Trim(), Is.EqualTo(ExpectedSourcePrefix.Trim()));
		}

		[Test]
		public void ShouldWriteSource()
		{
			FileCodeGeneratorContext context = CreateFileCodeGeneratorContext();
			context.WriteSource(writer =>
			{
				writer.WriteLine("Source Line #1.");
				writer.WriteLine("Source Line #2.");
			});

			Assert.That(context.DumpHeader().Trim(), Is.EqualTo(ExpectedHeaderPrefix));
			Assert.That(context.DumpSource().Trim(), Is.EqualTo($"""
			                                                    {ExpectedSourcePrefix}
			                                                    Source Line #1.
			                                                    Source Line #2.
			                                                    """));
		}
	}
}
