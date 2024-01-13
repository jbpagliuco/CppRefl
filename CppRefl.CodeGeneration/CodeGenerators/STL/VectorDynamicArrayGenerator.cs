using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators.STL
{
	internal class VectorDynamicArrayGenerator : ICodeGeneratorExtension
	{
		public void WriteClassSource(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
			var dynamicArrayFields = classInfo.Fields.Where(x => x.Type.QualifiedName.StartsWith("std::vector<"));
			if (dynamicArrayFields.Any())
			{
				using (writer.WithNamespace("CppReflPrivate"))
				{
					foreach (var fieldInfo in dynamicArrayFields)
					{
						string elementType = fieldInfo.Type.Template!.Arguments.First().ToString();
						string dynamicArrayFunctions = $"cpprefl::StdVectorFunctionsFactory::Create<{elementType}>(\"{elementType}\")";
						writer.WriteLine(CodeGeneratorUtil.RegisterDynamicArrayFunctions(classInfo, fieldInfo, dynamicArrayFunctions));
					}
				}
			}
		}
	}
}
