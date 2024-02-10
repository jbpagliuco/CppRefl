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
		public IReadOnlyList<string> Tags { get; init; } = new List<string>();

		/// <summary>
		/// Reflected attributes.
		/// </summary>
		public IReadOnlyDictionary<string, string> Attributes { get; init; } = new Dictionary<string, string>();

		/// <summary>
		/// Source information.
		/// </summary>
		public required SourceLocation SourceLocation { get; init; }

		/// <summary>
		/// Comments attached to this object.
		/// </summary>
		public required string? Comment { get; init; }
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
