using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators
{
	internal class FunctionStaticGenerator : ICodeGeneratorExtension
	{
		private string StaticFunctionSignature(FunctionInfo functionInfo) => $"const FunctionInfo& StaticFunction<&{functionInfo.GloballyQualifiedName}>()";

		public void WriteFunctionHeader(CodeWriter writer, FunctionInfo functionInfo, Registry registry)
		{
			// Forward declare
			var namespaceDec = functionInfo.IsInGlobalNamespace ? null : writer.WithNamespace(functionInfo.Namespace);
			writer.WriteLine($"{functionInfo.Signature};");
			namespaceDec?.Dispose();

			// StaticFunction<>()
			using (writer.WithNamespace("cpprefl"))
			{
				writer.WriteLine("template <>");
				writer.WriteLine(StaticFunctionSignature(functionInfo) + ";");
			}
		}

		public void WriteFunctionSource(CodeWriter writer, FunctionInfo functionInfo, Registry registry)
		{
			// StaticFunction<>()
			using (writer.WithNamespace("cpprefl"))
			{
				writer.WriteLine("template <>");
				using (writer.WithFunction(StaticFunctionSignature(functionInfo)))
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
								writer.WriteLine($"&{CodeGeneratorUtil.MaybeCreateStaticType(arg.Type)}");
							}
						}
					}

					// Register FunctionInfo
					using (writer.WithCodeBlock($"static const auto& functionInfo = cpprefl::Registry::GetSystemRegistry().AddFunction", "(", ");"))
					{
						using (writer.WithCodeBlock("FunctionInfo", "(", ")"))
						{
							using (writer.WithPostfix(","))
							{
								writer.WriteLine($"\"{functionInfo.QualifiedName}\"");
								writer.WriteLine($"(void*){functionInfo.QualifiedName}");
								writer.WriteLine($"{CodeGeneratorUtil.MaybeCreateStaticType(functionInfo.ReturnType.Type)}");
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

			// Instantiate
			using (writer.WithNamespace("CppReflPrivate"))
			{
				writer.WriteLine($"static const auto& {functionInfo.FlattenedName}_StaticFunction = cpprefl::StaticFunction<&{functionInfo.GloballyQualifiedName}>();");
			}
		}
	}
}
