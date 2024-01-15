using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators.STL
{
	internal class VectorDynamicArrayGenerator : IFileCodeGenerator
	{
		public void WriteClassSource(CppWriter writer, ClassInfo classInfo, Registry registry)
		{
			var dynamicArrayFields = classInfo.Fields.Where(x => x.Type.QualifiedName().StartsWith("std::vector<"));
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

		public void Execute(FileCodeGeneratorContext context)
		{
		}
	}
}
