namespace CppRefl.CodeGeneration.Reflection
{
	public class FunctionInfo : FunctionInfoBase
	{
		/// <summary>
		/// Function namespace.
		/// </summary>
		public required string Namespace { get; init; }



		/// <summary>
		/// An enumerable of all nested namespaces.
		/// </summary>
		public IEnumerable<string> Namespaces => Namespace.Split("::");

		/// <summary>
		/// Returns true if this type was defined in the global namespace.
		/// </summary>
		public bool IsInGlobalNamespace => Namespace.Length == 0;

		/// <summary>
		/// Returns the qualified name of this type, excluding the global namespace.
		/// </summary>
		public string QualifiedName => IsInGlobalNamespace ? Name : $"{Namespace}::{Name}";

		/// <summary>
		/// Returns the qualified name of this type, including the global namespace.
		/// </summary>
		public string GloballyQualifiedName => IsInGlobalNamespace ? $"::{Name}" : $"::{Namespace}::{Name}";

		/// <summary>
		/// Returns a fully qualified name using underscores instead of colons.
		/// </summary>
		public string FlattenedName => QualifiedName.Replace("::", "_");
	}
}