using CppRefl.CodeGeneration;
using CppRefl.CodeGeneration.Reflection;
using CppRefl.Compiler;

namespace CppRefl.UnitTest.CSharp.Tests
{
	internal class CodeGeneratorTests
	{
		[OneTimeSetUp]
		public void Setup()
		{
			var reflectedFiles = Directory.EnumerateFiles(TestUtil.Filepaths.ReflectedCodeDirectory);

			string registryFilename = Path.Combine(TestUtil.Filepaths.ReflectedCodeOutputDirectory, @"CppReflUnitTest.reflregistry.json");
			foreach (var file in reflectedFiles)
			{
				// Compile
				CompilerParams compilerParams = new()
				{
					SourceFileEntrypoint = file,
					ModulePath = TestUtil.Filepaths.ReflectedCodeDirectory,
					ModuleName = "CppReflUnitTest",
					IncludePaths = new[]
					{
						TestUtil.Filepaths.CppReflSourceDirectory,
						TestUtil.Filepaths.UnitTestCppDirectory
					},
					OutputDirectory = TestUtil.Filepaths.ReflectedCodeOutputDirectory,
					RegistryFilename = registryFilename
				};
				var compiler = new Compiler.Compiler(compilerParams);
				compiler.GenerateRegistry();

				// Generate
				CodeGeneratorFileParams codeGeneratorFileParams = new()
				{
					Registry = compiler.Registry,
					InputFilename = file,
					ModuleDirectory = TestUtil.Filepaths.ReflectedCodeDirectory,
					OutputDirectory = TestUtil.Filepaths.ReflectedCodeOutputDirectory
				};
				var codeGenerator = new CodeGenerator();
				codeGenerator.GenerateFileCode(codeGeneratorFileParams);
			}

			// Generate registry
			{
				CodeGeneratorRegistryParams codeGeneratorRegistryParams = new()
				{
					Registry = Registry.FromJson(File.ReadAllText(registryFilename))!,
					ModuleName = "CppReflUnitTest",
					ModuleDirectory = TestUtil.Filepaths.ReflectedCodeDirectory,
					OutputDirectory = TestUtil.Filepaths.ReflectedCodeOutputDirectory,
				};
				var codeGenerator = new CodeGenerator();
				codeGenerator.GenerateRegistryCode(codeGeneratorRegistryParams);
			}
		}

		[Test]
		public void Test()
		{
			
		}
	}
}
