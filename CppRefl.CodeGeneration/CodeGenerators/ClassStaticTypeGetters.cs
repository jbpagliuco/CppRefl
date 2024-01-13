using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators
{
	/// <summary>
	/// Generates Class::GetReflectedType, Class::ReflectedClass().
	/// </summary>
	internal class ClassStaticTypeGetters : ICodeGeneratorExtension
	{
		/// <summary>
		/// Generate code that belongs to the body of an existing class.
		/// </summary>
		/// <param name="writer"></param>
		/// <param name="classInfo"></param>
		/// <param name="registry"></param>
		public void WriteClassDeclaration(CodeWriter writer, ClassInfo classInfo, Registry registry)
		{
			if (classInfo.Type.IsInstantiable)
			{
				using (writer.WithPublic())
				{
					writer.WriteLine($$"""
					                   static inline const cpprefl::TypeInfo& StaticReflectedType() { return cpprefl::GetReflectedType<{{classInfo.Type.Name}}>(); }
					                   static inline const cpprefl::ClassInfo& StaticReflectedClass() { return cpprefl::GetReflectedClass<{{classInfo.Type.Name}}>(); }
					                   """);

					// Avoid writing virtual functions for non-class types. Structs are generally used for pure data, so we don't want mess up their memory format with a vtable pointer.
					if (classInfo.ClassType == ClassType.Class)
					{
						writer.WriteLine($$"""
						                   virtual const cpprefl::TypeInfo& GetReflectedType()const { return cpprefl::GetReflectedType<{{classInfo.Type.Name}}>(); }
						                   virtual const cpprefl::ClassInfo& GetReflectedClass()const { return cpprefl::GetReflectedClass<{{classInfo.Type.Name}}>(); }
						                   virtual const char* GetClassName()const { return cpprefl::GetTypeName<{{classInfo.Type.Name}}>(); }
						                   """);
					}
				}
			}
		}
	}
}
