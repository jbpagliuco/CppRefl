using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration
{
	public record FileObjects
	{
		/// <summary>
		/// List of reflected classes in the given file.
		/// </summary>
		public IEnumerable<ClassInfo> Classes { get; init; } = Enumerable.Empty<ClassInfo>();

		/// <summary>
		/// List of reflected enums in the given file.
		/// </summary>
		public IEnumerable<EnumInfo> Enums { get; init; } = Enumerable.Empty<EnumInfo>();

		/// <summary>
		/// List of reflected aliases in the given file.
		/// </summary>
		public IEnumerable<AliasInfo> Aliases { get; init; } = Enumerable.Empty<AliasInfo>();

		/// <summary>
		/// List of reflected functions in the given file.
		/// </summary>
		public IEnumerable<FunctionInfo> Functions { get; init; } = Enumerable.Empty<FunctionInfo>();

		public bool Any() => Classes.Any() || Enums.Any() || Aliases.Any() || Functions.Any();
	}
}
