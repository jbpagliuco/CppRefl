using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators
{
	/// <summary>
	/// Generates Class::Super.
	/// </summary>
	internal class ClassSuperGenerator : ICodeGeneratorExtension
	{
		public void WriteClassDeclaration(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
			if (classInfo.BaseClasses.Count > 0)
			{
				using (writer.WithPublic())
				{
					writer.WriteLine($"using Super = {classInfo.BaseClasses[0].Type.GloballyQualifiedName};");
				}
			}
		}
	}
}