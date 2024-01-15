using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators.Optional
{
    /// <summary>
    /// Generates Class::Super.
    /// </summary>
    internal class ClassSuperGenerator : ICodeGenerator
    {
        public void WriteClassDeclaration(CppWriter writer, ClassInfo classInfo, Registry registry)
        {
            var baseClass = classInfo.BaseClasses.FirstOrDefault(x => !x.IsAbstract);
            if (baseClass != null)
            {
                using (writer.WithPublic())
                {
                    writer.WriteLine($"using Super = {baseClass.Type.GloballyQualifiedName};");
                }
            }
        }
    }
}