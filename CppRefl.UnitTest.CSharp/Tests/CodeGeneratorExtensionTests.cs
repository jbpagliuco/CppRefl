using CppRefl.CodeGeneration;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.UnitTest.CSharp.Tests
{
	public class TestCodeGenerator : ICodeGeneratorExtension
	{
		public void WriteHeaderTop(CodeWriter writer, Registry registry)
		{
			writer.WriteLine("ClassFileStart");
		}

		public void WriteHeaderBottom(CodeWriter writer, Registry registry)
		{
			writer.WriteLine("ClassFileEnd");
		}

		public void WriteClassDeclaration(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
			writer.WriteLine($"Class: {classInfo.Type.Name}");
		}
	}

	internal class CodeGeneratorExtensionTests
	{
		//private Compiler.Compiler Compiler { get; } = new(TestUtil.CompilerParams);
		//private Registry Registry => Compiler.Registry;

		//private CodeGenerator CodeGenerator { get; set; } = null!;

		//private string OutputDirectory => Path.Combine(Path.GetTempPath(), nameof(CodeGeneratorExtensionTests));

		//[OneTimeSetUp]
		//public void Setup()
		//{
		//	Compiler.GenerateRegistry();

		//	CodeGenerator.LoadCodeGenerator(TestUtil.Filepaths.UnitTestCSharpDll);

		//	if (Directory.Exists(OutputDirectory))
		//	{
		//		Directory.Delete(OutputDirectory, true);
		//	}

		//	CodeGenerator = new(new CodeGeneratorParams()
		//	{
		//		Registry = Registry,
		//		InputFilename = TestUtil.Filepaths.ReflectedCodeFile,
		//		ModuleDirectory = TestUtil.Filepaths.UnitTestCppDirectory,
		//		OutputDirectory = OutputDirectory
		//	});
		//	CodeGenerator.GenerateCode();
		//}

		//[Test]
		//public void Test()
		//{
		//	string fileContents = File.ReadAllText(Path.Combine(OutputDirectory, $"ReflectedCode.reflgen.h")).Replace("\r\n", "\n");
		//	string expectedContents = File.ReadAllText(Path.Combine(TestUtil.Filepaths.UnitTestCSharpDirectory, "Tests", $"{nameof(CodeGeneratorExtensionTests)}.Expected.txt")).Replace("\r\n", "\n");
		//	Assert.That(fileContents, Is.EqualTo(expectedContents));
		//}
	}
}
