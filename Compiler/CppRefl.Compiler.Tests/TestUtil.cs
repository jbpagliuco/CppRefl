using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler.Tests
{
	internal static class TestUtil
	{
		internal static class Const
		{
			public static string SolutionDirectory => Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), "..", "..", "..", "..", "..", ".."));
			public static string CompilerDirectory => Path.Combine(SolutionDirectory, "Compiler");

			internal static class Runtime
			{
				/// <summary>
				/// Directory containing the "Runtime" modules.
				/// </summary>
				public static string Directory => Path.Combine(SolutionDirectory, "Runtime");

				/// <summary>
				/// Directory containing the "CppRefl" runtime source code.
				/// </summary>
				public static string SourceDirectory => Path.Combine(Directory, "CppRefl", "Source");

				internal static class Tests
				{
					/// <summary>
					/// CppRefl.Tests module name.
					/// </summary>
					public const string ModuleName = "CppReflTests";

					/// <summary>
					/// Directory containing the "CppRefl.Tests" source code.
					/// </summary>
					public static string SourceDirectory => Path.Combine(Directory, "CppRefl.Tests", "Source");

					/// <summary>
					/// Directory containing the "CppRefl.Tests" reflected headers.
					/// </summary>
					public static string ReflectedHeadersDirectory = Path.Combine(SourceDirectory, "ReflectedCode");

					/// <summary>
					/// Directory containing the "CppRefl.Tests" generated code.
					/// </summary>
					public static string GeneratedCodeDirectory => Path.Combine(SourceDirectory, "GeneratedCode");
				}
			}
		}

		public static CompilerParams CompilerParams(string file) => new()
		{
			SourceFileEntrypoint = file,
			ModulePath = Const.Runtime.Tests.ReflectedHeadersDirectory,
			ModuleName = "CppReflUnitTest",
			IncludePaths = new[]
			{
				Const.Runtime.SourceDirectory,
				Const.Runtime.Tests.SourceDirectory
			},
			OutputDirectory = Const.Runtime.Tests.GeneratedCodeDirectory,
			RegistryFilename = @"D:\test.json"
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
}
