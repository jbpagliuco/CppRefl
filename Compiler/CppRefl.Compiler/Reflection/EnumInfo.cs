namespace CppRefl.Compiler.Reflection
{
	public class EnumValueInfo : ObjectInfo
	{
		/// <summary>
		/// Name.
		/// </summary>
		public required string Name { get; init; }

		/// <summary>
		/// Enum integer value.
		/// </summary>
		public required long Value { get; init; }
	}

	/// <summary>
	/// Reflected enum.
	/// </summary>
	public class EnumInfo : ObjectInfo
	{
		/// <summary>
		/// Type information.
		/// </summary>
		public required TypeInfo Type { get; init; }

		/// <summary>
		/// Enum values.
		/// </summary>
		public IList<EnumValueInfo> Values { get; } = new List<EnumValueInfo>();

		/// <summary>
		/// Line number where the GENERATED_REFLECTION_CODE() macro is defined in this enum.
		/// </summary>
		public uint? GeneratedBodyLine { get; set; }

		public override string ToString() => Type.ToString();
	}
}
