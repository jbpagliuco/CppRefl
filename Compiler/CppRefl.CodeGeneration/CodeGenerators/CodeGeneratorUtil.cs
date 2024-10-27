using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;
using System.Text;

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
		public static string GetMetadataTagDefinitions(string name, MetadataInfo metadata)
		{
			StringBuilder sb = new();
			sb.Append($"static const std::array<cpprefl::MetadataTag, {metadata.RuntimeMetadataTags.Count()}> {name}Tags = {{");
			foreach (var value in metadata.RuntimeMetadataTags)
			{
				sb.Append($"\"{StripQuotes(value.Value)}\",");
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
		public static string WriteMetadataTagDefinitions(CodeWriter writer, string name, MetadataInfo metadata)
		{
			if (metadata.RuntimeMetadataTags.Any())
			{
				writer.WriteLine(GetMetadataTagDefinitions(name, metadata));
				return $"{name}Tags";
			}

			return "cpprefl::MetadataTagView()";
		}

		/// <summary>
		/// Create a string that defines metadata attributes.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="metadata"></param>
		/// <returns></returns>
		public static string GetMetadataAttributeDefinitions(string name, MetadataInfo metadata)
		{
			StringBuilder sb = new();
			sb.Append($"static const std::array<cpprefl::MetadataAttribute, {metadata.RuntimeMetadataAttributes.Count()}> {name}Attributes = {{");
			foreach (var pair in metadata.RuntimeMetadataAttributes)
			{
				var value = pair.Value.Value;

				// If this attribute doesn't have quotes, then see if it's convertable to a number. If not, add quotes.
				if (value[0] != '"')
				{
					if (!int.TryParse(value, out var _) && !float.TryParse(value, out var _))
					{
						value = $"\"{value}\"";
					}
				}

				sb.Append($"std::make_pair(\"{pair.Key}\",MetadataAttributeValue({value})),");
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
		public static string WriteMetadataAttributeDefinitions(CodeWriter writer, string name, MetadataInfo metadata)
		{
			if (metadata.RuntimeMetadataAttributes.Any())
			{
				writer.WriteLine(GetMetadataAttributeDefinitions(name, metadata));
				return $"{name}Attributes";
			}

			return "cpprefl::MetadataAttributeView()";
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
				return $"cpprefl::GetReflectedType<{typeInfo.QualifiedName()}>()";
			}

			return $"CppReflPrivate::MaybeCreateReflectedType<{typeInfo.QualifiedName()}>(\"{typeInfo.QualifiedName()}\")";
		}

		/// <summary>
		/// Registers a dynamic array generator.
		/// </summary>
		/// <returns></returns>
		public static string RegisterDynamicArrayFunctions(ClassInfo classInfo, FieldInfo fieldInfo, string dynamicArrayFunctions)
		{
			string varName = $"{classInfo.Type.FlattenedName()}{fieldInfo.Name}_DynamicArray";
			return $"const auto& {varName} = cpprefl::Registry::GetSystemRegistry().AddDynamicArrayFunctions(\"{fieldInfo.Type.QualifiedName()}\", {dynamicArrayFunctions});";
		}
	}
}
