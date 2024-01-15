namespace CppRefl.CodeGeneration.Reflection
{
	/// <summary>
	/// Metadata about a reflected object.
	/// </summary>
	public class MetadataInfo
	{
		/// <summary>
		/// Whether or not this object was explicitly reflected.
		/// </summary>
		public required bool IsReflected { get; init; }

		/// <summary>
		/// Reflected tags.
		/// </summary>
		public required IReadOnlyList<string> Tags { get; init; }

		/// <summary>
		/// Reflected attributes.
		/// </summary>
		public required IReadOnlyDictionary<string, string> Attributes { get; init; }

		/// <summary>
		/// Source information.
		/// </summary>
		public required SourceLocation SourceLocation { get; init; }
	}

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
