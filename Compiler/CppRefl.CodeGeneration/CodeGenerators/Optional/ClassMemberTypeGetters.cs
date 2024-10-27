using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators.Optional
{
	/// <summary>
	/// Generates Class::GetReflectedType(), Class::GetReflectedClass().
	/// </summary>
	internal class ClassMemberTypeGetters : IFileCodeGenerator
	{
		public void Execute(FileCodeGeneratorContext context)
		{
			foreach (var classInfo in context.Objects.Classes)
			{
				context.WriteClassDeclaration(classInfo, writer =>
				{
					using (writer.WithPublic())
					{
						// Avoid writing virtual functions for non-class types. Structs are generally used for pure data, so we don't want mess up their memory format with a vtable pointer.
						if (classInfo.ClassType == ClassType.Class)
						{
							writer.WriteLine($$"""
	                                            virtual const cpprefl::TypeInfo& GetReflectedType()const { return cpprefl::GetReflectedType<{{classInfo.Type.Name}}>(); }
	                                            virtual const cpprefl::ClassInfo& GetReflectedClass()const { return cpprefl::GetReflectedClass<{{classInfo.Type.Name}}>(); }
	                                            virtual const cpprefl::Name GetClassName()const { return cpprefl::GetTypeName<{{classInfo.Type.Name}}>(); }
	                                            """);
						}
					}
				});
			}
		}
	}
}
