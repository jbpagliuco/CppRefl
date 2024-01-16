using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators.Runtime
{
    internal class EnumReflectionGenerator : IFileCodeGenerator
    {
	    public void Execute(FileCodeGeneratorContext context)
        {
	        if (!context.Objects.Enums.Any())
	        {
		        return;
	        }

            context.WriteHeader(writer =>
            {
	            // For overloading GetReflectedType() and GetReflectedClass().
	            writer.IncludeHeader("CppReflStatics.h");

	            foreach (var enumInfo in context.Objects.Enums)
				{
					WriteEnumHeader(writer, enumInfo);
				}
            });

            context.WriteSource(writer =>
			{
				writer.IncludeHeader("Reflection/Registry.h");

				foreach (var enumInfo in context.Objects.Enums)
	            {
		            WriteEnumSource(writer, enumInfo);
	            }
			});

        }

	    public void WriteEnumHeader(CppWriter writer, EnumInfo enumInfo)
        {
            // Forward declare
            writer.ForwardDeclare(enumInfo);

            // Specialize the templated GetReflectedXX functions.
			using (writer.WithNamespace(CppDefines.Namespaces.Public))
            {
                writer.WriteLine($"""
                                  template <>
                                  const TypeInfo& GetReflectedType<{enumInfo.Type.GloballyQualifiedName()}>();
                                  
                                  template <>
                                  const EnumInfo& GetReflectedEnum<{enumInfo.Type.GloballyQualifiedName()}>();
                                  """);
            }
        }

	    public void WriteEnumSource(CppWriter writer, EnumInfo enumInfo)
        {
            using (writer.WithNamespace(CppDefines.Namespaces.Public))
            {
                writer.WriteLine($$"""
                                  template <>
                                  const TypeInfo& GetReflectedType<{{enumInfo.Type.GloballyQualifiedName()}}>()
                                  {
                                      static auto& type = cpprefl::Registry::GetSystemRegistry().AddType(cpprefl::TypeInfo("{{enumInfo.Type.QualifiedName()}}", cpprefl::TypeKind::Enum, sizeof({{enumInfo.Type.GloballyQualifiedName()}})));
                                      return type;
                                  }
                                  """);

                // Static enum
                writer.WriteLine("template <>");
                using (writer.WithFunction($"const EnumInfo& GetReflectedEnum<{enumInfo.Type.GloballyQualifiedName()}>()"))
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
                            writer.WriteLine($"cpprefl::EnumValueInfo(\"{value.Name}\", (int){enumInfo.Type.GloballyQualifiedName()}::{value.Name}, {tags}, {attributes}),");
                        }
                    }

                    using (writer.WithCodeBlock("static EnumInfo& enumInfo = cpprefl::Registry::GetSystemRegistry().AddEnum(EnumInfo", "(", "));"))
                    {
                        using (writer.WithPostfix(","))
                        {
                            writer.WriteLine($"GetReflectedType<{enumInfo.Type.GloballyQualifiedName()}>()");
                            writer.WriteLine($"{enumInfo.Type.Name}Values");
                            writer.WriteLine(enumTags);
                        }
                        writer.WriteLine(enumAttributes);
                    }

                    writer.WriteLine("return enumInfo;");
                }
            }
        }
    }
}
