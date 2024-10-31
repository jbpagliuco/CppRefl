using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.CodeGenerators.STL
{
	internal class VectorDynamicArrayGenerator : IFileCodeGenerator
	{
		public void Execute(FileCodeGeneratorContext context)
		{
			context.WriteSource(writer =>
			{
				foreach (var classInfo in context.Objects.Classes)
				{
					var dynamicArrayFields = classInfo.Fields.Where(x => x.Type.QualifiedName().StartsWith("std::vector<"));
					if (dynamicArrayFields.Any())
					{
						using (writer.WithNamespace(CppDefines.Namespaces.Private))
						{
							foreach (var fieldInfo in dynamicArrayFields)
							{
								string elementType = fieldInfo.Type.Template!.Arguments.First().ToString();
								string dynamicArrayFunctions = $"{CppDefines.Namespaces.Public}::StdVectorFunctionsFactory::Create<{elementType}>(cpprefl::EnsureName(\"{elementType}\"))";
								writer.WriteLine(CodeGeneratorUtil.RegisterDynamicArrayFunctions(classInfo, fieldInfo, dynamicArrayFunctions));
							}
						}
					}
				}
			});
		}
	}
}
