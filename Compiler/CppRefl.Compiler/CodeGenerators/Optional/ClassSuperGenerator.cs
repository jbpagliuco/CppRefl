using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.CodeGenerators.Optional
{
	/// <summary>
	/// Generates Class::Super.
	/// </summary>
	internal class ClassSuperGenerator : IFileCodeGenerator
	{
		public void Execute(FileCodeGeneratorContext context)
		{
			foreach (var classInfo in context.Objects.Classes)
			{
				var baseClass = classInfo.BaseClasses.FirstOrDefault();
				if (baseClass != null)
				{
					context.WriteClassDeclaration(classInfo, writer =>
					{
						using (writer.WithPublic())
						{
							writer.WriteLine($"using Super = {baseClass.Type.GloballyQualifiedName()};");
						}
					});
				}
			}
		}
	}
}