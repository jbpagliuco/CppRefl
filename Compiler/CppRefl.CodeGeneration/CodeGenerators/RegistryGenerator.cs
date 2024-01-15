using CppRefl.CodeGeneration.CodeWriters;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.CodeGenerators
{
	//internal class RegistryGenerator : ICodeGenerator
	//{
	//	/// <summary>
	//	/// Classes to reflect.
	//	/// </summary>
	//	/// <param name="registry"></param>
	//	/// <param name="params"></param>
	//	/// <returns></returns>
	//	private IEnumerable<ClassInfo> Classes(Registry registry, CodeGeneratorRegistryParams @params) => 
	//		registry.Classes.Values
	//			.Where(x => x.Type.IsInstantiable && x.Metadata.IsReflected && x.GeneratedBodyLine != null && x.Metadata.SourceLocation.Filepath.StartsWith(@params.ModuleDirectory));
		
	//	/// <summary>
	//	/// Enums to reflect.
	//	/// </summary>
	//	/// <param name="registry"></param>
	//	/// <returns></returns>
	//	private IEnumerable<EnumInfo> Enums(Registry registry) => registry.Enums.Values.Where(x => x.Metadata.IsReflected);

	//	/// <summary>
	//	/// Aliases to reflect.
	//	/// </summary>
	//	/// <param name="registry"></param>
	//	/// <returns></returns>
	//	private IEnumerable<AliasInfo> Aliases(Registry registry) => registry.Aliases.Values.Where(x => x.Metadata.IsReflected && x.AliasClass?.Type.Template?.IsSpecialized == true);
		

	//	public void WriteRegistrySource(CppWriter writer, Registry registry, CodeGeneratorRegistryParams @params)
	//	{
	//		HashSet<string> includeFilenames = Classes(registry, @params).Select(x => x.Metadata.SourceLocation.Filepath).ToHashSet();
	//		includeFilenames.UnionWith(Enums(registry).Select(x => x.Metadata.SourceLocation.Filepath));
	//		includeFilenames.UnionWith(Aliases(registry).Select(x => x.Metadata.SourceLocation.Filepath));

	//		foreach (var includeFilename in includeFilenames)
	//		{
	//			var include = Path.GetRelativePath(@params.ModuleDirectory, includeFilename);
	//			writer.IncludeHeader(include);
	//		}

	//		writer.WriteLine();
	//	}

	//	public void WriteRegistryInitializer(CppWriter writer, Registry registry, CodeGeneratorRegistryParams @params)
	//	{
	//		// Registry classes
	//		writer.WriteLine("// Registry classes");
	//		foreach (var classInfo in Classes(registry, @params))
	//		{
	//			writer.WriteLine($"cpprefl::GetReflectedClass<{classInfo.Type.QualifiedName}>();");
	//		}
	//		writer.WriteLine();

	//		// Registry enums
	//		writer.WriteLine("// Registry enums");
	//		foreach (var enumInfo in Enums(registry))
	//		{
	//			writer.WriteLine($"cpprefl::GetReflectedEnum<{enumInfo.Type.QualifiedName}>();");
	//		}
	//		writer.WriteLine();
	//	}
	//}
}
