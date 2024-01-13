using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators
{
	/// <summary>
	/// Generates Class::GetReflectedType, Class::StaticClass.
	/// </summary>
	internal class ClassStaticGenerator : ICodeGeneratorExtension
	{
		public void WriteHeaderTop(CodeWriter writer, FileObjects objects, Registry registry)
		{
			writer.IncludeHeader("CppReflStatics.h");
		}

		/// <summary>
		/// Generate code that belongs to the body of an existing class.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassDeclaration(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
			writer.WriteLine($"friend const cpprefl::TypeInfo& cpprefl::GetReflectedType<{classInfo.Type.Name}>();");
			writer.WriteLine($"friend const cpprefl::ClassInfo& cpprefl::StaticClass<{classInfo.Type.Name}>();");

			if (classInfo.Type.IsInstantiable)
			{
				using (writer.WithPublic())
				{
					writer.WriteLine($$"""
					                   static inline const cpprefl::TypeInfo& GetReflectedType() { return cpprefl::GetReflectedType<{{classInfo.Type.Name}}>(); }
					                   static inline const cpprefl::ClassInfo& StaticClass() { return cpprefl::StaticClass<{{classInfo.Type.Name}}>(); }
					                   static inline const char* StaticClassName() { return cpprefl::GetTypeName<{{classInfo.Type.Name}}>(); }
					                   """);

					// Avoid writing virtual functions for non-class types. Structs are generally used for pure data, so we don't want mess up their memory format with a vtable pointer.
					if (classInfo.ClassType == ClassType.Class)
					{
						writer.WriteLine($$"""
						                   virtual const cpprefl::TypeInfo& GetType()const { return cpprefl::GetReflectedType<{{classInfo.Type.Name}}>(); }
						                   virtual const cpprefl::ClassInfo& GetClass()const { return cpprefl::StaticClass<{{classInfo.Type.Name}}>(); }
						                   virtual const char* GetClassName()const { return cpprefl::GetTypeName<{{classInfo.Type.Name}}>(); }
						                   """);
					}
				}
			}
		}

		public void WriteClassHeader(CodeWriter writer, ClassInfo classInfo, string? name = null)
		{
			name ??= classInfo.Type.QualifiedName;

			// Forward declare
			if (classInfo.Type.Template?.IsSpecialized != true)
			{
				writer.WriteLine(classInfo.ForwardDeclaration);
			}

			using (writer.WithNamespace("cpprefl"))
			{
				// Static type
				writer.WriteLine("template <>");
				writer.WriteLine($"const TypeInfo& GetReflectedType<{name}>();");

				writer.WriteLine();

				// Static class
				writer.WriteLine("template <>");
				writer.WriteLine($"const ClassInfo& StaticClass<{name}>();");
			}
		}

		public void WriteClassHeader(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
			if (!classInfo.Type.IsTemplated)
			{
				WriteClassHeader(writer, classInfo);
			}
			else if (classInfo.Type.Template?.IsGeneric == true)
			{
				writer.WriteLine(classInfo.ForwardDeclaration);
			}
		}

		/// <summary>
		/// Writes code at the top of a generated file containing class info.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="registry"></param>
		public void WriteSourceTop(CodeWriter writer, FileObjects objects, Registry registry)
		{
			writer.WriteLine($"""
			                  #include "Reflection/ClassInfo.h"
			                  #include "Reflection/FieldInfo.h"
			                  #include "Reflection/Registry.h"
			                  #include "Reflection/TypeInfo.h"
			                  
			                  """);
		}

		private void WriteClassSource(CodeWriter writer, ClassInfo classInfo, string? name = null)
		{
			name ??= classInfo.Type.QualifiedName;

			using (writer.WithNamespace("cpprefl"))
			{
				// Static type
				writer.WriteLine("template <>");
				using (writer.WithFunction($"const TypeInfo& GetReflectedType<{name}>()"))
				{
					writer.WriteLine(
						$"static auto& type = cpprefl::Registry::GetSystemRegistry().AddType(cpprefl::TypeInfo(\"{name}\", cpprefl::TypeKind::Class, sizeof({name})));");
					writer.WriteLine("return type;");
				}

				writer.WriteLine();

				// Static class
				writer.WriteLine("template <>");
				using (writer.WithFunction(
						   $"const ClassInfo& StaticClass<{name}>()"))
				{
					string classTags = CodeGeneratorUtil.WriteTagDefinitions(writer, "Class", classInfo.Metadata);
					string classAttributes =
						CodeGeneratorUtil.WriteAttributeDefinitions(writer, "Class", classInfo.Metadata);

					// Write field info data.
					if (classInfo.Fields.Count > 0)
					{
						Dictionary<FieldInfo, string> fieldTags = new();
						Dictionary<FieldInfo, string> fieldAttributes = new();
						foreach (var field in classInfo.Fields)
						{
							fieldTags[field] =
								CodeGeneratorUtil.WriteTagDefinitions(writer, field.Name, field.Metadata);
							fieldAttributes[field] =
								CodeGeneratorUtil.WriteAttributeDefinitions(writer, field.Name, field.Metadata);
						}

						using (writer.WithCodeBlock(
								   $"static const std::array<cpprefl::FieldInfo, {classInfo.Fields.Count}> Fields =",
								   "{", "};"))
						{
							foreach (var field in classInfo.Fields)
							{
								writer.WriteLine($"""
									                  cpprefl::FieldInfo(
									                  	cpprefl::MakeTypeInstance<decltype({name}::{field.Name})>({CodeGeneratorUtil.MaybeCreateReflectedType(field.Type)}),
									                  	offsetof({name}, {field.Name}),
									                  	"{field.Name}",
									                  	{fieldTags[field]},
									                  	{fieldAttributes[field]}
									                  ),
									                  """);
							}
						}
					}

					using (writer.WithCodeBlock(
							   "static ClassInfo& classInfo = cpprefl::Registry::GetSystemRegistry().AddClass(ClassInfo",
							   "(", "));"))
					{
						using (writer.WithPostfix(","))
						{
							var baseClass = classInfo.BaseClasses.FirstOrDefault();
							while (baseClass != null && baseClass.Metadata.IsReflected && baseClass.Type.IsTemplated)
							{
								baseClass = baseClass.BaseClasses.FirstOrDefault();
							}

							var ctor = classInfo.IsAbstract ? "nullptr" : $"[](void * obj) {{ new(obj) {name}(); }}";
							var dtor = classInfo.IsAbstract ? "nullptr" : $"[](void * obj) {{ (({name}*)obj)->~{classInfo.Type.Name}(); }}";

							writer.WriteLine($"GetReflectedType<{name}>()");
							writer.WriteLine(
								baseClass != null && baseClass.Metadata.IsReflected && !baseClass.Type.IsTemplated
									? $"&StaticClass<{baseClass.Type.GloballyQualifiedName}>()"
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

		/// <summary>
		/// Generate code that defines a class.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassSource(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
			if (!classInfo.Type.IsTemplated)
			{
				WriteClassSource(writer, classInfo);
			}
		}

		public void WriteAliasHeader(CodeWriter writer, AliasInfo aliasInfo, Registry registry)
		{
			if (aliasInfo.AliasClass != null && aliasInfo.AliasClass.Type.Template?.IsSpecialized == true)
			{
				WriteClassHeader(writer, aliasInfo.AliasClass);
			}
		}

		public void WriteAliasSource(CodeWriter writer, AliasInfo aliasInfo, Registry registry)
		{
			if (aliasInfo.AliasClass != null && aliasInfo.AliasClass.Type.Template?.IsSpecialized == true)
			{
				WriteClassSource(writer, aliasInfo.AliasClass, aliasInfo.Type.QualifiedName);
			}
		}
	}
}
