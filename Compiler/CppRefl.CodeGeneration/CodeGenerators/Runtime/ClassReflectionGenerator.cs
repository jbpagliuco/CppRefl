using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators.Runtime
{
	/// <summary>
	/// Generates Class::GetReflectedType, Class::ReflectedClass().
	/// </summary>
	internal class ClassReflectionGenerator : IFileCodeGenerator
	{
		//public void WriteAliasHeader(CppWriter writer, AliasInfo aliasInfo, Registry registry)
		//{
		//    if (aliasInfo.AliasClass != null && aliasInfo.AliasClass.Type.Template?.IsSpecialized == true)
		//    {
		//        WriteClassHeader(writer, aliasInfo.AliasClass);
		//    }
		//}

		//public void WriteAliasSource(CppWriter writer, AliasInfo aliasInfo, Registry registry)
		//{
		//    if (aliasInfo.AliasClass != null && aliasInfo.AliasClass.Type.Template?.IsSpecialized == true)
		//    {
		//        WriteClassSource(writer, aliasInfo.AliasClass, aliasInfo.Type.QualifiedName);
		//    }
		//}

		public void Execute(FileCodeGeneratorContext context)
		{
			if (!context.Objects.Classes.Any())
			{
				return;
			}

			context.WriteHeader(writer =>
			{
				// For overloading GetReflectedType() and GetReflectedClass().
				writer.IncludeHeader("CppReflStatics.h");

				foreach (var classInfo in context.Objects.Classes)
				{
					WriteClassHeader(writer, classInfo);
				}
			});

			foreach (var classInfo in context.Objects.Classes)
			{
				context.WriteClassDeclaration(classInfo, writer =>
				{
					// Make these functions friends since they may need to access the offset of any private fields.
					writer.WriteLine($"friend const cpprefl::TypeInfo& cpprefl::GetReflectedType<{classInfo.Type.Name}>();");
					writer.WriteLine($"friend const cpprefl::ClassInfo& cpprefl::GetReflectedClass<{classInfo.Type.Name}>();");
				});
			}

			context.WriteSource(writer =>
			{
				writer.IncludeHeader("Reflection/ClassInfo.h");
				writer.IncludeHeader("Reflection/FieldInfo.h");
				writer.IncludeHeader("Reflection/Registry.h");
				writer.IncludeHeader("Reflection/TypeInfo.h");

				foreach (var classInfo in context.Objects.Classes)
				{
					WriteClassSource(writer, classInfo);
				}
			});
		}

		public void WriteClassHeader(CppWriter writer, ClassInfo classInfo)
		{
			if (classInfo.Type.IsTemplated)
			{
				if (classInfo.Type.Template!.IsGeneric)
				{
					// Just forward declare generic templates to make life a little easier.
					writer.ForwardDeclare(classInfo);
				}

				return;
			}

			writer.ForwardDeclare(classInfo);

			// Specialize the templated GetReflectedXX functions.
			using (writer.WithNamespace(CppDefines.Namespaces.Public))
			{
				writer.WriteLine("template <>");
				writer.WriteLine($"const TypeInfo& GetReflectedType<{classInfo.Type.GloballyQualifiedName()}>();");

				writer.WriteLine();

				writer.WriteLine("template <>");
				writer.WriteLine($"const ClassInfo& GetReflectedClass<{classInfo.Type.GloballyQualifiedName()}>();");
			}
		}

		private void WriteClassSource(CppWriter writer, ClassInfo classInfo)
		{
			if (classInfo.Type.IsTemplated)
			{
				return;
			}

			using (writer.WithNamespace(CppDefines.Namespaces.Public))
			{
				// Static type
				writer.WriteLine("template <>");
				using (writer.WithFunction($"const TypeInfo& GetReflectedType<{classInfo.Type.GloballyQualifiedName()}>()"))
				{
					writer.WriteLine(
						$"static auto& type = cpprefl::Registry::GetSystemRegistry().EmplaceType(\"{classInfo.Type.QualifiedName()}\", cpprefl::TypeKind::Class, sizeof({classInfo.Type.GloballyQualifiedName()}));");
					writer.WriteLine("return type;");
				}

				writer.WriteLine();

				// Static class
				writer.WriteLine("template <>");
				using (writer.WithFunction(
						   $"const ClassInfo& GetReflectedClass<{classInfo.Type.GloballyQualifiedName()}>()"))
				{
					string classTags = CodeGeneratorUtil.WriteMetadataTagDefinitions(writer, "Class", classInfo.Metadata);
					string classAttributes =
						CodeGeneratorUtil.WriteMetadataAttributeDefinitions(writer, "Class", classInfo.Metadata);

					// Write field info data.
					if (classInfo.Fields.Count > 0)
					{
						Dictionary<FieldInfo, string> fieldTags = new();
						Dictionary<FieldInfo, string> fieldAttributes = new();
						foreach (var field in classInfo.Fields)
						{
							fieldTags[field] =
								CodeGeneratorUtil.WriteMetadataTagDefinitions(writer, field.Name, field.Metadata);
							fieldAttributes[field] =
								CodeGeneratorUtil.WriteMetadataAttributeDefinitions(writer, field.Name, field.Metadata);
						}

						using (writer.WithCodeBlock(
								   $"static const std::array<cpprefl::FieldInfo, {classInfo.Fields.Count}> Fields =",
								   "{", "};"))
						{
							foreach (var field in classInfo.Fields)
							{
								writer.WriteLine($"""
									                  cpprefl::FieldInfo(
									                  	cpprefl::MakeTypeInstance<decltype({classInfo.Type.GloballyQualifiedName()}::{field.Name})>({CodeGeneratorUtil.MaybeCreateReflectedType(field.Type)}),
									                  	offsetof({classInfo.Type.GloballyQualifiedName()}, {field.Name}),
									                  	"{field.Name}",
									                  	{fieldTags[field]},
									                  	{fieldAttributes[field]}
									                  ),
									                  """);
							}
						}
					}

					using (writer.WithCodeBlock(
							   "static const auto& classInfo = cpprefl::Registry::GetSystemRegistry().EmplaceClass",
							   "(", ");"))
					{
						using (writer.WithPostfix(","))
						{
							var baseClass = classInfo.BaseClasses.FirstOrDefault();
							while (baseClass != null && baseClass.Metadata.IsReflected && baseClass.Type.IsTemplated)
							{
								baseClass = baseClass.BaseClasses.FirstOrDefault();
							}

							var ctor = classInfo.IsAbstract ? "nullptr" : $"[](void * obj) {{ new(obj) {classInfo.Type.GloballyQualifiedName()}(); }}";
							var dtor = classInfo.IsAbstract ? "nullptr" : $"[](void * obj) {{ (({classInfo.Type.GloballyQualifiedName()}*)obj)->~{classInfo.Type.Name}(); }}";

							writer.WriteLine($"&GetReflectedType<{classInfo.Type.GloballyQualifiedName()}>()");
							writer.WriteLine(
								baseClass != null && baseClass.Metadata.IsReflected && !baseClass.Type.IsTemplated
									? $"&GetReflectedClass<{baseClass.Type.GloballyQualifiedName()}>()"
									: "nullptr");
							writer.WriteLine(ctor);
							writer.WriteLine(dtor);
							writer.WriteLine(classInfo.Fields.Count > 0 ? "Fields" : "cpprefl::FieldView()");
							writer.WriteLine(classTags);
						}

						writer.WriteLine(classAttributes);
					}

					writer.WriteLine("return classInfo;");
				}
			}
		}
	}
}
