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
            var reflectedFiles = TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory.EnumerateFiles();

            string registryFilename = Path.Combine(TestUtil.Const.Runtime.Tests.GeneratedCodeDirectory.FullName, $"{TestUtil.Const.Runtime.Tests.ModuleName}.reflregistry.json");
            foreach (var file in reflectedFiles)
            {
                // Compile
                CompilerParams compilerParams = new()
                {
                    SourceFileEntrypoint = file,
                    ModuleDirectory = TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory,
                    ModuleName = TestUtil.Const.Runtime.Tests.ModuleName,
                    IncludePaths = new[]
                    {
                        TestUtil.Const.Runtime.SourceDirectory.FullName,
                        TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory.FullName,
                        TestUtil.Const.Runtime.Tests.GeneratedCodeDirectory.FullName,
                    },
                    OutputDirectory = TestUtil.Const.Runtime.Tests.GeneratedCodeDirectory,
                    RegistryFilename = registryFilename
                };
                var compiler = new Compiler(compilerParams);
                compiler.GenerateRegistry();

                // Generate
                var codeGenerator = new CodeGenerator();
                CodeGeneratorFileParams codeGeneratorFileParams = new()
                {
                    Registry = compiler.Registry,
                    InputFilename = file,
                    ModuleDirectory = TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory,
                    OutputDirectory = TestUtil.Const.Runtime.Tests.GeneratedCodeDirectory
                };
                codeGenerator.GenerateFileCode(codeGeneratorFileParams);
            }

            // Generate registry
            {
                CodeGeneratorModuleParams codeGeneratorRegistryParams = new()
                {
                    Registry = Registry.FromJson(File.ReadAllText(registryFilename))!,
                    ModuleName = TestUtil.Const.Runtime.Tests.ModuleName,
                    ModuleDirectory = TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory,
                    OutputDirectory = TestUtil.Const.Runtime.Tests.GeneratedCodeDirectory,
                };
                var codeGenerator = new CodeGenerator();
                codeGenerator.GenerateModuleCode(codeGeneratorRegistryParams);
            }
        }
    }
}
