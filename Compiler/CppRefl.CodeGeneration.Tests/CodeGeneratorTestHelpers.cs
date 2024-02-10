using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.Tests
{
	internal static class CodeGeneratorTestHelpers
	{
		/// <summary>
		/// Creates an empty class info.
		/// </summary>
		/// <returns></returns>
		public static ClassInfo CreateClassInfo()
		{
			return new ClassInfo()
			{
				Type = new()
				{
					Name = "Class",
					Namespace = "Namespace"
				},
				Metadata = new()
				{
					IsReflected = true,
					SourceLocation = new(Path.Combine(Path.GetTempPath(), "Class.h"), 123),
					Comment = ""
				},
				ClassType = ClassType.Class,
				IsAbstract = false,
				GeneratedBodyLine = 125
			};
		}

		/// <summary>
		/// Creates an empty class info.
		/// </summary>
		/// <returns></returns>
		public static EnumInfo CreateEnumInfo()
		{
			return new()
			{
				Type = new()
				{
					Name = "Enum",
					Namespace = "Namespace"
				},
				Metadata = new()
				{
					IsReflected = true,
					SourceLocation = new(Path.Combine(Path.GetTempPath(), "Enum.h"), 123),
					Comment = ""
				},
				GeneratedBodyLine = 246
			};
		}
	}
}
