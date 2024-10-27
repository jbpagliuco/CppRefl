using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.Tests.Reflection
{
    internal static class TestUtil
    {
        public static CompilerParams CompilerParams(FileInfo file) => new()
        {
            InputFile = file,
            ModuleDirectory = Const.Runtime.Tests.ReflectedHeadersDirectory,
            ModuleName = "CppReflUnitTest",
            IncludePaths = new[]
            {
                Const.Runtime.SourceDirectory.FullName,
                Const.Runtime.Tests.SourceDirectory.FullName
            },
            OutputDirectory = Const.Runtime.Tests.GeneratedCodeDirectory
        };

        /// <summary>
        /// Tests a type.
        /// </summary>
        /// <param name="type"></param>
        /// <param name="name"></param>
        /// <param name="kind"></param>
        /// <param name="isPrimitive"></param>
        /// <param name="isInteger"></param>
        /// <param name="isReal"></param>
        /// <param name="isUnsigned"></param>
        /// <param name="isSigned"></param>
        /// <param name="isClass"></param>
        /// <param name="isEnum"></param>
        /// <param name="typeNamespace"></param>
        public static void TestType(TypeInfo type, string name, TypeKind kind,
            bool isPrimitive = false,
            bool isInteger = false,
            bool isReal = false,
            bool isUnsigned = false,
            bool isSigned = false,
            bool isClass = false,
            bool isEnum = false,
            string typeNamespace = "")
        {
            Assert.IsNotNull(type);
            Assert.That(type.Name, Is.EqualTo(name));
            Assert.That(type.Kind, Is.EqualTo(kind));
            Assert.That(type.IsPrimitive, Is.EqualTo(isPrimitive));
            Assert.That(type.IsInteger, Is.EqualTo(isInteger));
            Assert.That(type.IsReal, Is.EqualTo(isReal));
            Assert.That(type.IsUnsigned, Is.EqualTo(isUnsigned));
            Assert.That(type.IsSigned, Is.EqualTo(isSigned));
            Assert.That(type.IsClass, Is.EqualTo(isClass));
            Assert.That(type.IsEnum, Is.EqualTo(isEnum));
            Assert.That(type.Namespace, Is.EqualTo(typeNamespace));

            if (isPrimitive)
            {
                Assert.IsFalse(type.ClangCursor.IsValid());
                Assert.IsTrue(type.ClangType.IsValid());
            }
            else
            {
                Assert.IsTrue(type.ClangCursor.IsValid());
                Assert.IsTrue(type.ClangType.IsValid());
            }
        }

        /// <summary>
        /// Test a type instance info.
        /// </summary>
        /// <param name="typeInstanceInfo"></param>
        /// <param name="typeInfo"></param>
        /// <param name="isConst"></param>
        public static void TestTypeInstanceInfo(TypeInstanceInfo typeInstanceInfo, TypeInfo typeInfo,
            bool isConst = false)
        {
            Assert.That(typeInstanceInfo.Type, Is.EqualTo(typeInfo));
            Assert.That(typeInstanceInfo.Const, Is.EqualTo(isConst));
        }
    }

    internal static class Const
    {
        public static DirectoryInfo SolutionDirectory => new(Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), "..", "..", "..", "..", "..", "..")));
        public static DirectoryInfo CompilerDirectory => new(Path.Combine(SolutionDirectory.FullName, "Compiler"));

        internal static class Runtime
        {
            /// <summary>
            /// Directory containing the "Runtime" modules.
            /// </summary>
            public static DirectoryInfo Directory => new(Path.Combine(SolutionDirectory.FullName, "Runtime"));

            /// <summary>
            /// Directory containing the "CppRefl" runtime source code.
            /// </summary>
            public static DirectoryInfo SourceDirectory => new(Path.Combine(Directory.FullName, "CppRefl", "Source"));

            internal static class Tests
            {
                /// <summary>
                /// CppRefl.Tests module name.
                /// </summary>
                public const string ModuleName = "CppReflTests";

                /// <summary>
                /// Directory containing the "CppRefl.Tests" reflected headers.
                /// </summary>
                public static DirectoryInfo ReflectedHeadersDirectory = new(Path.Combine(SourceDirectory.FullName, "ReflectedCode"));

                /// <summary>
                /// Directory containing the "CppRefl.Tests" source code.
                /// </summary>
                public static DirectoryInfo SourceDirectory => new(Path.Combine(Directory.FullName, "CppRefl.Tests", "Source"));

                /// <summary>
                /// Directory containing the "CppRefl.Tests" generated code.
                /// </summary>
                public static DirectoryInfo GeneratedCodeDirectory => new(Path.Combine(Directory.FullName, "CppRefl.Tests", "GeneratedCode"));
            }
        }
    }
}
