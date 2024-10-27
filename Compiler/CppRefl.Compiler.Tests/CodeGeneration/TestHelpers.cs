using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.Tests.CodeGeneration
{
	public static class TestHelpers
	{
		public static SourceLocation CreateSourceLocation() => new(("D:\\cpprefl\\testhelpers\\file.h"), 1234);

		public static MetadataInfo CreateMetadataInfo()
		{
			return new()
			{
				SourceLocation = CreateSourceLocation(),
				IsReflected = true,
				Comment = null
			};
		}

		public static TypeInfo CreateTypeInfo(string name)
		{
			return new()
			{
				Name = name,
				Namespace = "Namespace"
			};
		}

		public static TypeInstanceInfo CreateTypeInstanceInfo(string typeName)
		{
			return new()
			{
				Type = CreateTypeInfo(typeName),
				Const = false
			};
		}

		public static FunctionInfo CreateFunctionInfo(string name)
		{
			return new()
			{
				Name = name,
				Namespace = "Namespace",
				Metadata = CreateMetadataInfo(),
				ReturnType = CreateTypeInstanceInfo("void")
			};
		}
	}
}
