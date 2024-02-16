namespace CppRefl.CodeGeneration.Reflection
{
	/// <summary>
	/// Defines when a metadata value is defined.
	/// </summary>
	[Flags]
	public enum MetadataValueLifetime
	{
		/// <summary>
		/// Metadata is only available at compile-time.
		/// </summary>
		Compile = 1,

		/// <summary>
		/// Metadata is available at both compile-time and runtime.
		/// </summary>
		Runtime = 3
	}

	/// <summary>
	/// A metadata value.
	/// </summary>
	public record MetadataValue
	{
		/// <summary>
		/// When this metadata value is accessible.
		/// </summary>
		public required MetadataValueLifetime Lifetime { get; init; }

		/// <summary>
		/// The metadata value.
		/// </summary>
		public required string Value { get; init; }
	}

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
		public IReadOnlyList<MetadataValue> Tags { get; init; } = new List<MetadataValue>();

		/// <summary>
		/// Reflected attributes.
		/// </summary>
		public IReadOnlyDictionary<string, MetadataValue> Attributes { get; init; } = new Dictionary<string, MetadataValue>();

		/// <summary>
		/// Source information.
		/// </summary>
		public required SourceLocation SourceLocation { get; init; }

		/// <summary>
		/// Comments attached to this object.
		/// </summary>
		public required string? Comment { get; init; }

		/// <summary>
		/// Metadata that is only available at compile-time.
		/// </summary>
		public IEnumerable<MetadataValue> CompileMetadataTags => Tags.Where(x => x.Lifetime.HasFlag(MetadataValueLifetime.Compile));

		/// <summary>
		/// Metadata that is available at both compile-time and runtime.
		/// </summary>
		public IEnumerable<MetadataValue> RuntimeMetadataTags => Tags.Where(x => x.Lifetime.HasFlag(MetadataValueLifetime.Runtime));

		/// <summary>
		/// Metadata that is only available at compile-time.
		/// </summary>
		public IEnumerable<KeyValuePair<string, MetadataValue>> CompileMetadataAttributes => Attributes.Where(x => x.Value.Lifetime.HasFlag(MetadataValueLifetime.Compile));

		/// <summary>
		/// Metadata that is available at both compile-time and runtime.
		/// </summary>
		public IEnumerable<KeyValuePair<string, MetadataValue>> RuntimeMetadataAttributes => Attributes.Where(x => x.Value.Lifetime.HasFlag(MetadataValueLifetime.Runtime));
	}
}
