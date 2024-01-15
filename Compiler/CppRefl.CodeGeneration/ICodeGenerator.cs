using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration
{
	/// <summary>
	/// Implements a code generator for a file.
	/// </summary>
	public interface IFileCodeGenerator
	{
		public void Execute(FileCodeGeneratorContext context);
	}

	/// <summary>
	/// Implements a code generator for a module.
	/// </summary>
	public interface IModuleCodeGenerator
	{
		public void Execute(ModuleCodeGeneratorContext context);
	}
}
