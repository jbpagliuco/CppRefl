using System.Text;
using CppRefl.CodeGeneration.Reflection;
using FieldInfo = CppRefl.CodeGeneration.Reflection.FieldInfo;
using TypeInfo = CppRefl.CodeGeneration.Reflection.TypeInfo;

namespace CppRefl.CodeGeneration.CodeGenerators
{
	public static class CodeGeneratorUtil
	{
		private static string StripQuotes(string s) => s.Trim('"');

		/// <summary>
		/// Create a string that defines metadata tags.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="metadata"></param>
		/// <returns></returns>
		public static string GetTagDefinitions(string name, MetadataInfo metadata)
		{
			StringBuilder sb = new();
			sb.Append($"static constexpr std::array<cpprefl::TagType, {metadata.Tags.Count}> {name}Tags = {{");
			foreach (var tag in metadata.Tags)
			{
				sb.Append($"\"{StripQuotes(tag)}\",");
			}
			sb.Append("};");

			return sb.ToString();
		}

		/// <summary>
		/// Writes the definitions for metadata tags.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="name"></param>
		/// <param name="metadata"></param>
		/// <returns></returns>
		public static string WriteTagDefinitions(CodeWriter writer, string name, MetadataInfo metadata)
		{
			if (metadata.Tags.Count > 0)
			{
				writer.WriteLine(GetTagDefinitions(name, metadata));
				return $"{name}Tags";
			}

			return "cpprefl::TagView()";
		}

		/// <summary>
		/// Create a string that defines metadata attributes.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="metadata"></param>
		/// <returns></returns>
		public static string GetAttributeDefinitions(string name, MetadataInfo metadata)
		{
			StringBuilder sb = new();
			sb.Append($"static constexpr std::array<cpprefl::AttributeType, {metadata.Attributes.Count}> {name}Attributes = {{");
			foreach (var attribute in metadata.Attributes)
			{
				var value = attribute.Value;
				
				// If this attribute doesn't have quotes, then see if it's convertable to a number. If not, add quotes.
				if (value[0] != '"')
				{
					if (!int.TryParse(value, out var _) && !float.TryParse(value, out var _))
					{
						value = $"\"{value}\"";
					}
				}

				sb.Append($"std::make_pair(\"{attribute.Key}\",{value}),");
			}
			sb.Append("};");

			return sb.ToString();
		}

		/// <summary>
		/// Writes the definitions for metadata attributes.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="name"></param>
		/// <param name="metadata"></param>
		/// <returns></returns>
		public static string WriteAttributeDefinitions(CodeWriter writer, string name, MetadataInfo metadata)
		{
			if (metadata.Attributes.Count > 0)
			{
				writer.WriteLine(GetAttributeDefinitions(name, metadata));
				return $"{name}Attributes";
			}

			return "cpprefl::AttributeView()";
		}

		/// <summary>
		/// Create a "MaybeCreateReflectedType" invocation.
		/// </summary>
		/// <param name="typeInfo"></param>
		/// <returns></returns>
		public static string MaybeCreateReflectedType(TypeInfo typeInfo)
		{
			if (typeInfo.IsPrimitive)
			{
				return $"cpprefl::GetReflectedType<{typeInfo.QualifiedName}>()";
			}
			
			return $"CppReflPrivate::MaybeCreateReflectedType<{typeInfo.QualifiedName}>(\"{typeInfo.QualifiedName}\")";
		}

		/// <summary>
		/// Registers a dynamic array generator.
		/// </summary>
		/// <returns></returns>
		public static string RegisterDynamicArrayFunctions(ClassInfo classInfo, FieldInfo fieldInfo, string dynamicArrayFunctions)
		{
			string varName = $"{classInfo.Type.FlattenedName}{fieldInfo.Name}_DynamicArray";
			return $"const auto& {varName} = cpprefl::Registry::GetSystemRegistry().AddDynamicArrayFunctions(\"{fieldInfo.Type.QualifiedName}\", {dynamicArrayFunctions});";
		}
	}
}
