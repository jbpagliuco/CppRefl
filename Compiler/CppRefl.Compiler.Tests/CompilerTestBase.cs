using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler.Tests
{
	internal abstract class CompilerTestBase
	{
		protected Compiler Compiler { get; set; } = null!;
		protected Registry Registry => Compiler.Registry;

		protected abstract string ReflectionFile { get; }

		[OneTimeSetUp]
		public void GenerateReflection()
		{
			CompilerParams compilerParams = new()
			{
				SourceFileEntrypoint = ReflectionFile,
				ModulePath = TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory,
				ModuleName = TestUtil.Const.Runtime.Tests.ModuleName,
				IncludePaths = new[]
				{
					TestUtil.Const.Runtime.SourceDirectory,
					TestUtil.Const.Runtime.Tests.SourceDirectory,
					TestUtil.Const.Runtime.Tests.GeneratedCodeDirectory
				},
				// TODO: Create a "no output" mode.
				OutputDirectory = TestUtil.Const.Runtime.Tests.GeneratedCodeDirectory
			};
			Compiler = new Compiler(compilerParams);

			Compiler.GenerateRegistry();
		}
	}
}
