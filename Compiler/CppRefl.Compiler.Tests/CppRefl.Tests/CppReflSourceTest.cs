using CppRefl.CodeGeneration;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler.Tests.CppRefl.Tests
{
    /// <summary>
    /// This is not a traditional NUnit test, as it doesn't assert anything. It can be used to:
    ///		1. Easily generate code for our CppRefl.Tests program (just run this test from the Test Explorer).
    ///		2. Debug generation of a real program.
    /// </summary>
    internal class CppReflSourceTest
    {
	    [Test]
        public void Generate()
        {
            var reflectedFiles = Const.Runtime.Tests.ReflectedHeadersDirectory.EnumerateFiles();

            string registryFilename = Path.Combine(Const.Runtime.Tests.GeneratedCodeDirectory.FullName, $"{Const.Runtime.Tests.ModuleName}.reflregistry.json");
            foreach (var file in reflectedFiles)
            {
                // Compile
                CompilerParams compilerParams = new()
                {
                    InputFile = file,
                    ModuleDirectory = Const.Runtime.Tests.ReflectedHeadersDirectory,
                    ModuleName = Const.Runtime.Tests.ModuleName,
                    IncludePaths = new[]
                    {
                        Const.Runtime.SourceDirectory.FullName,
                        Const.Runtime.Tests.ReflectedHeadersDirectory.FullName,
                        Const.Runtime.Tests.GeneratedCodeDirectory.FullName,
                    },
                    OutputDirectory = Const.Runtime.Tests.GeneratedCodeDirectory
                };
                var compiler = new Compiler(compilerParams);
                compiler.GenerateRegistry();

                // Generate
                var codeGenerator = new CodeGenerator();
                CodeGeneratorFileParams codeGeneratorFileParams = new()
                {
                    Registry = compiler.Registry,
                    InputFilename = file,
                    ModuleDirectory = Const.Runtime.Tests.ReflectedHeadersDirectory,
                    OutputDirectory = Const.Runtime.Tests.GeneratedCodeDirectory
                };
                codeGenerator.GenerateFileCode(codeGeneratorFileParams);
            }

            // Generate registry
            {
                CodeGeneratorModuleParams codeGeneratorRegistryParams = new()
                {
                    Registry = Registry.CollectFileRegistries(Const.Runtime.Tests.ModuleName, Const.Runtime.Tests.ReflectedHeadersDirectory, Const.Runtime.Tests.GeneratedCodeDirectory),
                    ModuleName = Const.Runtime.Tests.ModuleName,
                    ModuleDirectory = Const.Runtime.Tests.ReflectedHeadersDirectory,
                    OutputDirectory = Const.Runtime.Tests.GeneratedCodeDirectory,
                };
                var codeGenerator = new CodeGenerator();
                codeGenerator.GenerateModuleCode(codeGeneratorRegistryParams);
            }
        }
    }
}
