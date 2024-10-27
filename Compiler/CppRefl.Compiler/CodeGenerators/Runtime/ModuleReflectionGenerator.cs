using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.CodeGenerators.Runtime
{
	internal class ModuleReflectionGenerator : IModuleCodeGenerator
	{
		public void Execute(ModuleCodeGeneratorContext context)
		{
			HashSet<ObjectInfo> reflectedObjects = new();

			// Classes
			var classes = context.Parameters.Registry.GetClassesWithinModule(context.Parameters.ModuleDirectory)
				.Where(x => x.Type.IsInstantiable &&
							x.Metadata.IsReflected &&
							x.GeneratedBodyLine != null);
			reflectedObjects.UnionWith(classes);
			foreach (var classInfo in classes)
			{
				context.WriteInitializer(writer => writer.WriteLine($"cpprefl::GetReflectedClass<{classInfo.Type.QualifiedName()}>();"));
			}

			// Enums
			var enums = context.Parameters.Registry.GetEnumsWithinModule(context.Parameters.ModuleDirectory)
				.Where(x => x.Metadata.IsReflected);
			reflectedObjects.UnionWith(enums);
			foreach (var enumInfo in enums)
			{
				context.WriteInitializer(writer => writer.WriteLine($"cpprefl::GetReflectedEnum<{enumInfo.Type.QualifiedName()}>();"));
			}

			// Functions
			var functions = context.Parameters.Registry.GetFunctionsWithinModule(context.Parameters.ModuleDirectory)
				.Where(x => x.Metadata.IsReflected);
			reflectedObjects.UnionWith(functions);
			foreach (var functionInfo in functions)
			{
				context.WriteInitializer(writer => writer.WriteLine($"cpprefl::GetReflectedFunction<&{functionInfo.QualifiedName()}>();"));
			}

			// Add includes.
			context.WriteSource(writer =>
			{
				var includes = reflectedObjects
					.Select(x =>
						Path.GetRelativePath(context.Parameters.ModuleDirectory.FullName, x.Metadata.SourceLocation.FileInfo.FullName))
					.ToHashSet();

				foreach (var include in includes)
				{
					writer.IncludeHeader(include);
				}
			});
		}
	}
}
