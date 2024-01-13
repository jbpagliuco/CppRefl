using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators
{
	internal class AliasStaticGenerator : ICodeGeneratorExtension
	{
		public void WriteAliasHeader(CodeWriter writer, AliasInfo aliasInfo, Registry registry)
		{
			if (aliasInfo.AliasType.IsTemplated)
			{
				// Forward declare
				var namespaceDec = aliasInfo.Type.IsInGlobalNamespace ? null : writer.WithNamespace(aliasInfo.Type.Namespace);
				writer.WriteLine($"enum class {aliasInfo.Type.Name};");
				namespaceDec?.Dispose();

				using (writer.WithNamespace("cpprefl"))
				{
					// Static type
					writer.WriteLine("template <>");
					writer.WriteLine($"const TypeInfo& StaticType<{aliasInfo.Type.GloballyQualifiedName}>();");

					writer.WriteLine();

					// Static enum
					writer.WriteLine("template <>");
					writer.WriteLine($"const EnumInfo& StaticEnum<{aliasInfo.Type.GloballyQualifiedName}>();");
				}
			}
		}

		public void WriteAliasSource(CodeWriter writer, AliasInfo aliasInfo, Registry registry)
		{
			throw new NotImplementedException();
		}
	}
}
