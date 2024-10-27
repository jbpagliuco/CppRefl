namespace CppRefl.Compiler.Reflection
{
	/// <summary>
	/// Base class of all reflected objects (classes, properties, functions, etc.)
	/// </summary>
	public class ObjectInfo
	{
		/// <summary>
		/// Metadata.
		/// </summary>
		public required MetadataInfo Metadata { get; init; }
	}
}
