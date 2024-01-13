using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators
{
	internal class EnumStaticGenerator : ICodeGeneratorExtension
	{
		private string EnumNamespace(EnumInfo enumInfo) => enumInfo.Type.IsInGlobalNamespace ? "CppReflPrivate::Enums" : $"CppReflPrivate::Enums::{enumInfo.Type.Namespace}";
		private string MetadataNamespace(EnumInfo enumInfo) => enumInfo.Type.IsInGlobalNamespace ? "CppReflPrivate::Metadata" : $"CppReflPrivate::Metadata::{enumInfo.Type.Namespace}";

		public void WriteEnumHeader(CodeWriter writer, EnumInfo enumInfo, Registry registry)
		{
			// Forward declare
			var namespaceDec = enumInfo.Type.IsInGlobalNamespace ? null : writer.WithNamespace(enumInfo.Type.Namespace);
			writer.WriteLine($"enum class {enumInfo.Type.Name};");
			namespaceDec?.Dispose();

			using (writer.WithNamespace("cpprefl"))
			{
				// Static type
				writer.WriteLine("template <>");
				writer.WriteLine($"const TypeInfo& StaticType<{enumInfo.Type.GloballyQualifiedName}>();");

				writer.WriteLine();

				// Static enum
				writer.WriteLine("template <>");
				writer.WriteLine($"const EnumInfo& StaticEnum<{enumInfo.Type.GloballyQualifiedName}>();");
			}
		}

		/// <summary>
		/// Generate code for an enum in a source file.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="enumInfo"></param>
		/// <param name="registry"></param>
		public void WriteEnumSource(CodeWriter writer, EnumInfo enumInfo, Registry registry)
		{
			using (writer.WithNamespace("cpprefl"))
			{
				// Static type
				writer.WriteLine("template <>");
				using (writer.WithFunction($"const TypeInfo& StaticType<{enumInfo.Type.GloballyQualifiedName}>()"))
				{
					writer.WriteLine(
						$"static auto& type = cpprefl::Registry::GetSystemRegistry().AddType(cpprefl::TypeInfo(\"{enumInfo.Type.QualifiedName}\", cpprefl::TypeKind::Enum, sizeof({enumInfo.Type.QualifiedName})));");
					writer.WriteLine("return type;");
				}

				writer.WriteLine();

				// Static enum
				writer.WriteLine("template <>");
				using (writer.WithFunction($"const EnumInfo& StaticEnum<{enumInfo.Type.GloballyQualifiedName}>()"))
				{
					string enumTags = CodeGeneratorUtil.WriteTagDefinitions(writer, "Enum", enumInfo.Metadata);
					string enumAttributes = CodeGeneratorUtil.WriteAttributeDefinitions(writer, "Enum", enumInfo.Metadata);

					// List of enum info values
					using (writer.WithCodeBlock(
							   $"static const std::array<cpprefl::EnumValueInfo, {enumInfo.Values.Count}> {enumInfo.Type.Name}Values =",
							   "{", "};"))
					{
						foreach (var value in enumInfo.Values)
						{
							string tags = value.Metadata.Tags.Count > 0 ? $"{enumInfo.Type.Name}::{value.Name}Tags" : "cpprefl::TagView()";
							string attributes = value.Metadata.Attributes.Count > 0 ? $"{enumInfo.Type.Name}::{value.Name}Attributes" : "cpprefl::AttributeView()";
							writer.WriteLine($"cpprefl::EnumValueInfo(\"{value.Name}\", (int){enumInfo.Type.GloballyQualifiedName}::{value.Name}, {tags}, {attributes}),");
						}
					}

					using (writer.WithCodeBlock("static EnumInfo& enumInfo = cpprefl::Registry::GetSystemRegistry().AddEnum(EnumInfo", "(", "));"))
					{
						using (writer.WithPostfix(","))
						{
							writer.WriteLine($"StaticType<{enumInfo.Type.GloballyQualifiedName}>()");
							writer.WriteLine($"{enumInfo.Type.Name}Values");
							writer.WriteLine(enumTags);
						}
						writer.WriteLine(enumAttributes);
					}

					writer.WriteLine("return enumInfo;");
				}
			}

			// Instantiate
			using (writer.WithNamespace("CppReflPrivate"))
			{
				writer.WriteLine($"static const auto& {enumInfo.Type.FlattenedName}_StaticType = cpprefl::StaticType<{enumInfo.Type.GloballyQualifiedName}>();");
				writer.WriteLine($"static const auto& {enumInfo.Type.FlattenedName}_StaticEnum = cpprefl::StaticEnum<{enumInfo.Type.GloballyQualifiedName}>();");
			}
		}
	}
}
