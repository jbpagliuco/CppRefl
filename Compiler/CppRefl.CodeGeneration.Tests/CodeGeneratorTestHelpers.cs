﻿using CppRefl.CodeGeneration.Reflection;

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
					SourceLocation = new()
					{
						Filepath = Path.Combine(Path.GetTempPath(), "Class.h"),
						Line = 123
					}
				},
				ClassType = ClassType.Class,
				IsAbstract = false,
				GeneratedBodyLine = 125
			};
		}
	}
}
