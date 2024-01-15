using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;
using Microsoft.Win32;

namespace CppRefl.CodeGeneration.CodeGenerators.Runtime
{
    internal class ModuleReflectionGenerator : IModuleCodeGenerator
    {
        public void Execute(ModuleCodeGeneratorContext context)
        {
	        HashSet<ObjectInfo> reflectedObjects = new();

			// Classes
			var classes = context.Parameters.Registry.GetClassesWithinModule(new DirectoryInfo(context.Parameters.ModuleDirectory))
	            .Where(x => x.Type.IsInstantiable && 
	                        x.Metadata.IsReflected && 
	                        x.GeneratedBodyLine != null);
			reflectedObjects.UnionWith(classes);
			foreach (var classInfo in classes)
			{
				context.WriteInitializer(writer => writer.WriteLine($"cpprefl::GetReflectedClass<{classInfo.Type.QualifiedName()}>();"));
            }

            // Enums
            var enums = context.Parameters.Registry.GetEnumsWithinModule(new DirectoryInfo(context.Parameters.ModuleDirectory))
	            .Where(x => x.Metadata.IsReflected);
            reflectedObjects.UnionWith(enums);
			foreach (var classInfo in classes)
            {
	            context.WriteInitializer(writer => writer.WriteLine($"cpprefl::GetReflectedClass<{classInfo.Type.QualifiedName()}>();"));
            }

            // Add includes.
            context.WriteSource(writer =>
            {
	            var includes = reflectedObjects
		            .Select(x =>
			            Path.GetRelativePath(context.Parameters.ModuleDirectory, x.Metadata.SourceLocation.Filepath))
		            .ToHashSet();

	            foreach (var include in includes)
	            {
                    writer.IncludeHeader(include);
				}
            });
		}
    }
}
