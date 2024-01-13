using CppRefl.CodeGeneration.Reflection;
using CppRefl.Compiler;

namespace CppRefl.UnitTest.CSharp
{
	internal static class TestUtil
	{
		internal static class Filepaths
		{
			public static string SolutionDirectory => Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), "..", "..", "..", "..", ".."));

			public static string CppReflDirectory => Path.Combine(SolutionDirectory, "CppRefl");
			public static string CppReflSourceDirectory => Path.Combine(CppReflDirectory, "Source");

			public static string UnitTestCSharpDirectory => Path.Combine(SolutionDirectory, "CppRefl.UnitTest.CSharp");
			public static string UnitTestCSharpDll => Path.Combine(Directory.GetCurrentDirectory(), "CppRefl.UnitTest.CSharp.dll");

			public static string UnitTestCppDirectory => Path.Combine(SolutionDirectory, "CppRefl.UnitTest.Cpp");
			public static string ReflectedCodeDirectory => Path.Combine(UnitTestCppDirectory, "ReflectedCode");
			public static string ReflectedCodeFile => Path.Combine(ReflectedCodeDirectory, "ReflectedCode.h");
			public static string ReflectedCodeOutputDirectory => Path.Combine(UnitTestCppDirectory, "GeneratedCode");
		}

		public static CompilerParams CompilerParams(string file) => new()
		{
			SourceFileEntrypoint = file,
			ModulePath = Filepaths.ReflectedCodeDirectory,
			ModuleName = "CppReflUnitTest",
			IncludePaths = new[]
			{
				Filepaths.CppReflSourceDirectory,
				Filepaths.UnitTestCppDirectory
			},
			OutputDirectory = Filepaths.ReflectedCodeOutputDirectory,
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
