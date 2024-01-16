using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators.Runtime
{
    internal class FunctionReflectionGenerator : IFileCodeGenerator
    {
        public void WriteFunctionHeader(CppWriter writer, FunctionInfo functionInfo)
        {
            writer.ForwardDeclare(functionInfo);

            using (writer.WithNamespace(CppDefines.Namespaces.Public))
            {
	            writer.WriteLine($"""
	                              template <>
	                              const FunctionInfo& GetReflectedFunction<&{functionInfo.GloballyQualifiedName()}>();
	                              """);
            }
        }

        public void WriteFunctionSource(CppWriter writer, FunctionInfo functionInfo)
        {
			// StaticFunction<>()
			using (writer.WithNamespace(CppDefines.Namespaces.Public))
			{
                writer.WriteLine("template <>");
                using (writer.WithFunction($"const FunctionInfo& GetReflectedFunction<&{functionInfo.GloballyQualifiedName()}>()"))
                {
                    string functionTags = CodeGeneratorUtil.WriteTagDefinitions(writer, "Function", functionInfo.Metadata);
                    string functionAttributes = CodeGeneratorUtil.WriteAttributeDefinitions(writer, "Function", functionInfo.Metadata);

                    // Function arguments
                    if (functionInfo.ArgumentTypes.Any())
                    {
                        using (writer.WithCodeBlock($"static const std::array<const TypeInfo*, {functionInfo.ArgumentTypes.Count}> functionArgs", "{", "};"))
                        {
                            using var _ = writer.WithPostfix(",");
                            foreach (var arg in functionInfo.ArgumentTypes)
                            {
                                writer.WriteLine($"&{CodeGeneratorUtil.MaybeCreateReflectedType(arg.Type)}");
                            }
                        }
                    }

                    // Registry FunctionInfo
                    using (writer.WithCodeBlock($"static const auto& functionInfo = cpprefl::Registry::GetSystemRegistry().AddFunction", "(", ");"))
                    {
                        using (writer.WithCodeBlock("FunctionInfo", "(", ")"))
                        {
                            using (writer.WithPostfix(","))
                            {
                                writer.WriteLine($"\"{functionInfo.QualifiedName()}\"");
                                writer.WriteLine($"(void*){functionInfo.QualifiedName()}");
                                writer.WriteLine($"{CodeGeneratorUtil.MaybeCreateReflectedType(functionInfo.ReturnType.Type)}");
                                writer.WriteLine(functionInfo.ArgumentTypes.Any()
                                    ? "FunctionArgTypesView(functionArgs)"
                                    : "FunctionArgTypesView()");
                                writer.WriteLine(functionTags);
                            }
                            writer.WriteLine(functionAttributes);
                        }
                    }

                    writer.WriteLine("return functionInfo;");
                }
            }
        }

        public void Execute(FileCodeGeneratorContext context)
		{
			if (!context.Objects.Functions.Any())
			{
				return;
			}

			context.WriteHeader(writer => writer.IncludeHeader("CppReflStatics.h"));
			context.WriteSource(writer =>
			{
				writer.IncludeHeader("Reflection/FunctionInfo.h");
				writer.IncludeHeader("Reflection/Registry.h");
			});

			foreach (var functionInfo in context.Objects.Functions)
			{
				context.WriteHeader(writer => WriteFunctionHeader(writer, functionInfo));
				context.WriteSource(writer => WriteFunctionSource(writer, functionInfo));
			}
		}
    }
}
