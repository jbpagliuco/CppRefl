namespace CppRefl.CodeGeneration.CodeGenerators.Optional
{
    /// <summary>
    /// Generates Class::StaticReflectedType(), Class::StaticReflectedClass().
    /// </summary>
    internal class ClassStaticTypeGetters : IFileCodeGenerator
    {
	    public void Execute(FileCodeGeneratorContext context)
        {
	        foreach (var classInfo in context.Objects.Classes)
	        {
		        context.WriteClassDeclaration(classInfo, writer =>
		        {
			        using (writer.WithPublic())
			        {
				        writer.WriteLine($$"""
				                           static inline const cpprefl::TypeInfo& StaticReflectedType() { return cpprefl::GetReflectedType<{{classInfo.Type.Name}}>(); }
				                           static inline const cpprefl::ClassInfo& StaticReflectedClass() { return cpprefl::GetReflectedClass<{{classInfo.Type.Name}}>(); }
				                           """);
			        }
		        });
	        }
        }
    }
}
