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

	public class TestModuleCodeGenerator : IModuleCodeGenerator
	{
		public bool Executed { get; private set; } = false;

		public void Execute(ModuleCodeGeneratorContext context)
		{
			context.WriteHeader(writer => writer.Write("Header"));
			context.WriteInitializer(writer => writer.Write("Initializer"));
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

		private CodeGeneratorModuleParams CreateModuleGeneratorParams()
		{
			var root = Path.GetTempPath();
			return new()
			{
				Registry = new(),
				ModuleName = "ModuleName",
				ModuleDirectory = $@"{root}Source",
				OutputDirectory = $@"{root}Generated"
			};
		}
		

		[Test]
		public void ShouldProduceCorrectFilenames()
		{
			CodeGenerator generator = new();

			{
				var result = generator.GenerateFileCode(CreateFileGeneratorParams(), _ => new MemoryStream());

				Assert.That(result.Header.Filename,
					Is.EqualTo($@"{Path.GetTempPath()}Generated\Subfolder\File.reflgen.h"));
				Assert.That(result.Source.Filename,
					Is.EqualTo($@"{Path.GetTempPath()}Generated\Subfolder\File.reflgen.cpp"));
			}

			{
				var result = generator.GenerateModuleCode(CreateModuleGeneratorParams(), _ => new MemoryStream());

				Assert.That(result.Header.Filename,
					Is.EqualTo($@"{Path.GetTempPath()}Generated\ModuleName.reflgen.h"));
				Assert.That(result.Source.Filename,
					Is.EqualTo($@"{Path.GetTempPath()}Generated\ModuleName.reflgen.cpp"));
			}
		}




		[Test]
		public void ShouldExecuteFileGenerator()
		{
			CodeGenerator generator = new();

			var testGenerator = generator.AddFileCodeGenerator<TestFileCodeGenerator>();
			Assert.False(testGenerator.Executed);

			generator.GenerateFileCode(CreateFileGeneratorParams(), _ => new MemoryStream());
			Assert.True(testGenerator.Executed);
		}

		[Test]
		public void ShouldLoadFileGeneratorFromAssembly()
		{
			CodeGenerator generator = new();
			generator.FileGenerators.Clear();
			generator.LoadCodeGenerators(Assembly.GetAssembly(typeof(TestFileCodeGenerator))!);
			Assert.That(generator.FileGenerators.Select(x => x.GetType()), Is.EquivalentTo(new[]{ typeof(TestFileCodeGenerator)}));
		}

		[Test]
		public void ShouldWriteFileContents()
		{
			CodeGenerator generator = new();
			generator.FileGenerators.Clear();
			generator.AddFileCodeGenerator<TestFileCodeGenerator>();
			var result = generator.GenerateFileCode(CreateFileGeneratorParams(), _ => new MemoryStream());

			var headerStream = result.Header.Stream as MemoryStream;
			var sourceStream = result.Source.Stream as MemoryStream;

			Assert.That(Encoding.UTF8.GetString(headerStream!.ToArray()).Trim(), 
				Is.EqualTo($"""
				            #undef {CppDefines.FileId}
				            #define {CppDefines.FileId} File
				            
				            #if defined(File_REFLGEN_H)
				                #error Including File.reflgen.h multiple times! Use `#pragma once` in File.h.
				            #endif
				            
				            #define File_REFLGEN_H
				            
				            #include "Private/CppReflGeneratedCodeMacros.h"
				            Header
				            """));

			Assert.That(Encoding.UTF8.GetString(sourceStream!.ToArray()).Trim(), 
				Is.EqualTo($"""
				            #include "Subfolder/File.h" 
				            Source
				            """));
		}





		[Test]
		public void ShouldExecuteModuleGenerator()
		{
			CodeGenerator generator = new();

			var testGenerator = generator.AddModuleCodeGenerator<TestModuleCodeGenerator>();
			Assert.False(testGenerator.Executed);

			generator.GenerateModuleCode(CreateModuleGeneratorParams(), _ => new MemoryStream());
			Assert.True(testGenerator.Executed);
		}

		[Test]
		public void ShouldLoadModuleGeneratorFromAssembly()
		{
			CodeGenerator generator = new();
			generator.ModuleGenerators.Clear();
			generator.LoadCodeGenerators(Assembly.GetAssembly(typeof(TestModuleCodeGenerator))!);
			Assert.That(generator.ModuleGenerators.Select(x => x.GetType()), Is.EquivalentTo(new[] { typeof(TestModuleCodeGenerator) }));
		}

		[Test]
		public void ShouldWriteModuleContents()
		{
			CodeGenerator generator = new();
			generator.ModuleGenerators.Clear();
			generator.AddModuleCodeGenerator<TestModuleCodeGenerator>();
			var result = generator.GenerateModuleCode(CreateModuleGeneratorParams(), _ => new MemoryStream());

			var headerStream = result.Header.Stream as MemoryStream;
			var sourceStream = result.Source.Stream as MemoryStream;

			Assert.That(Encoding.UTF8.GetString(headerStream!.ToArray()).Trim(),
				Is.EqualTo("""
				             #pragma once
				             
				             // Register the reflection runtime information for this module.
				             void RegisterModuleNameReflection();
				             
				             Header
				             """));

			Assert.That(Encoding.UTF8.GetString(sourceStream!.ToArray()).Trim(),
				Is.EqualTo($$"""
				            #include "ModuleName.reflgen.h"
				            
				            #include "CppReflStatics.h"
				            
				            Source
				            
				            void RegisterModuleNameReflection()
				            {
				            	Initializer
				            }
				            """));
		}
	}
}
