using CppRefl.CodeGeneration.Reflection;
using CppRefl.Compiler;

namespace CppRefl.UnitTest.CSharp.Tests.CompilerTests
{
	internal abstract class CompilerTestBase
	{
		protected Compiler.Compiler Compiler { get; set; } = null!;
		protected Registry Registry => Compiler.Registry;

		protected abstract string ReflectionFile { get; }

		[OneTimeSetUp]
		public void GenerateReflection()
		{
			CompilerParams compilerParams = new()
			{
				SourceFileEntrypoint = ReflectionFile,
				ModulePath = TestUtil.Filepaths.ReflectedCodeDirectory,
				ModuleName = "CppReflUnitTest",
				IncludePaths = new[]
				{
					TestUtil.Filepaths.CppReflSourceDirectory,
					TestUtil.Filepaths.UnitTestCppDirectory
				},
				OutputDirectory = TestUtil.Filepaths.ReflectedCodeOutputDirectory
			};
			Compiler = new Compiler.Compiler(compilerParams);

			Compiler.GenerateRegistry();
		}
	}
}
