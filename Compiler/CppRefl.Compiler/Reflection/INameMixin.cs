namespace CppRefl.Compiler.Reflection
{
	public interface INameMixin
	{
		/// <summary>
		/// Name of this instance.
		/// </summary>
		public string Name { get; }

		/// <summary>
		/// Namespace this instance belongs to.
		/// </summary>
		public string Namespace { get; }
	}

	public static class NameMixinStatics
	{
		/// <summary>
		/// An enumerable of all nested namespaces.
		/// </summary>
		public static IEnumerable<string> Namespaces(this INameMixin inst) => inst.IsInGlobalNamespace() ? Enumerable.Empty<string>() : inst.Namespace.Split("::");

		/// <summary>
		/// Returns true if this type was defined in the global namespace.
		/// </summary>
		public static bool IsInGlobalNamespace(this INameMixin inst) => inst.Namespace.Length == 0;

		/// <summary>
		/// Returns the qualified name of this type, excluding the global namespace.
		/// </summary>
		public static string QualifiedName(this INameMixin inst) => inst.IsInGlobalNamespace() ? inst.Name : $"{inst.Namespace}::{inst.Name}";

		/// <summary>
		/// Returns the qualified name of this type, including the global namespace.
		/// </summary>
		public static string GloballyQualifiedName(this INameMixin inst) => inst.IsInGlobalNamespace() ? $"::{inst.Name}" : $"::{inst.Namespace}::{inst.Name}";

		/// <summary>
		/// Returns a fully qualified name using underscores instead of colons.
		/// </summary>
		public static string FlattenedName(this INameMixin inst) => inst.QualifiedName().Replace("::", "_");
	}
}
