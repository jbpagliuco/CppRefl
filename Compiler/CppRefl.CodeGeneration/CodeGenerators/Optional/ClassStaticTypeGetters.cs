using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators.Optional
{
    /// <summary>
    /// Generates Class::StaticReflectedType(), Class::StaticReflectedClass().
    /// </summary>
    internal class ClassStaticTypeGetters : ICodeGenerator
    {
        /// <summary>
        /// Generate code that belongs to the body of an existing class.
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="classInfo"></param>
        /// <param name="registry"></param>
        public void WriteClassDeclaration(CppWriter writer, ClassInfo classInfo, Registry registry)
        {
            if (classInfo.Type.IsInstantiable)
            {
                using (writer.WithPublic())
                {
                    writer.WriteLine($$"""
					                   static inline const cpprefl::TypeInfo& StaticReflectedType() { return cpprefl::GetReflectedType<{{classInfo.Type.Name}}>(); }
					                   static inline const cpprefl::ClassInfo& StaticReflectedClass() { return cpprefl::GetReflectedClass<{{classInfo.Type.Name}}>(); }
					                   """);
                }
            }
        }
    }
}
