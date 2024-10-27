using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.Tests.Reflection
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
                InputFile = new FileInfo(ReflectionFile),
                ModuleDirectory = Const.Runtime.Tests.ReflectedHeadersDirectory,
                ModuleName = Const.Runtime.Tests.ModuleName,
                IncludePaths = new[]
                {
                    Const.Runtime.SourceDirectory.FullName,
                    Const.Runtime.Tests.SourceDirectory.FullName,
                    Const.Runtime.Tests.GeneratedCodeDirectory.FullName
                },
                OutputDirectory = Const.Runtime.Tests.GeneratedCodeDirectory
            };
            Compiler = new Compiler(compilerParams);

            Compiler.GenerateRegistry();
        }
    }
}
