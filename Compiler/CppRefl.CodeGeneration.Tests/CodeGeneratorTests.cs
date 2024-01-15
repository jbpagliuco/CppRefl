using System.Reflection;
using System.Text;

namespace CppRefl.CodeGeneration.Tests
{
	public class TestFileCodeGenerator : IFileCodeGenerator
	{
		public bool Executed { get; private set; } = false;

		public void Execute(FileCodeGeneratorContext context)
		{
			context.WriteHeader(writer => writer.Write("Header"));
			context.WriteSource(writer => writer.Write("Source"));
			Executed = true;
		}
	}
	
	internal class CodeGeneratorTests
	{
		private CodeGeneratorFileParams CreateFileGeneratorParams()
		{
			var root = Path.GetTempPath();
			return new()
			{
				Registry = new(),
				InputFilename = $@"{root}Source/Subfolder/File.h",
				ModuleDirectory = $@"{root}Source",
				OutputDirectory = $@"{root}Generated"
			};
		}

		private string ExpectedHeaderPrefix =>
			$"""
			 #undef {CppDefines.FileId}
			 #define {CppDefines.FileId} File

			 #if defined(File_REFLGEN_H)
			     #error Including {Path.GetTempPath()}Source/Subfolder/File.h multiple times! Use `#pragma once` in File.h.
			 #endif

			 #define File_REFLGEN_H

			 #include "Private/CppReflGeneratedCodeMacros.h"
			 """;


		private string ExpectedSourcePrefix => "#include \"Subfolder/File.h\" ";

		[Test]
		public void ShouldProduceCorrectFilenames()
		{
			CodeGenerator generator = new();
			var result = generator.GenerateFileCode(CreateFileGeneratorParams(), _ => new MemoryStream());

			Assert.That(result.Header.Filename, Is.EqualTo($@"{Path.GetTempPath()}Generated\Subfolder\File.reflgen.h"));
			Assert.That(result.Source.Filename, Is.EqualTo($@"{Path.GetTempPath()}Generated\Subfolder\File.reflgen.cpp"));
		}

		[Test]
		public void ShouldExecuteFileGenerator()
		{
			CodeGenerator generator = new();

			var testGenerator = generator.AddCodeGenerator<TestFileCodeGenerator>();
			Assert.False(testGenerator.Executed);

			generator.GenerateFileCode(CreateFileGeneratorParams(), _ => new MemoryStream());
			Assert.True(testGenerator.Executed);
		}

		[Test]
		public void ShouldLoadFileGeneratorFromAssembly()
		{
			CodeGenerator generator = new();
			generator.LoadCodeGenerators(Assembly.GetAssembly(typeof(TestFileCodeGenerator))!);
			Assert.That(generator.FileGenerators.Select(x => x.GetType()), Is.EquivalentTo(new[]{ typeof(TestFileCodeGenerator)}));
		}

		[Test]
		public void ShouldWriteFileContents()
		{
			CodeGenerator generator = new();
			generator.AddCodeGenerator<TestFileCodeGenerator>();
			var result = generator.GenerateFileCode(CreateFileGeneratorParams(), _ => new MemoryStream());

			var headerStream = result.Header.Stream as MemoryStream;
			var sourceStream = result.Source.Stream as MemoryStream;

			Assert.That(Encoding.UTF8.GetString(headerStream!.ToArray()).Trim(), Is.EqualTo($"{ExpectedHeaderPrefix}{Environment.NewLine}Header"));
			Assert.That(Encoding.UTF8.GetString(sourceStream!.ToArray()).Trim(), Is.EqualTo($"{ExpectedSourcePrefix}{Environment.NewLine}Source"));
		}
	}
}
