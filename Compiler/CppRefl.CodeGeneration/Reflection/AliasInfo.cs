namespace CppRefl.CodeGeneration.Reflection
{
	public class AliasInfo : ObjectInfo
	{
		/// <summary>
		/// Our type info.
		/// </summary>
		public required TypeInfo Type { get; init; }

		/// <summary>
		/// The type info we're aliased as.
		/// </summary>
		public required TypeInfo AliasType { get; init; }

		/// <summary>
		/// The class we're directly aliased as.
		/// </summary>
		public ClassInfo? AliasClass { get; init; }

		/// <summary>
		/// The enum we're directly aliased as.
		/// </summary>
		public EnumInfo? AliasEnum { get; init; }

		public ClassInfo? GetUnderlyingClass(Registry registry) => AliasClass ?? registry.GetAlias(AliasType.QualifiedName)?.GetUnderlyingClass(registry);
		public EnumInfo? GetUnderlyingEnum(Registry registry) => AliasEnum ?? registry.GetAlias(AliasType.QualifiedName)?.GetUnderlyingEnum(registry);
	}
}
