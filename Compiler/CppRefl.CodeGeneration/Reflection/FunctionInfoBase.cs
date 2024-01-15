namespace CppRefl.CodeGeneration.Reflection
{
	public abstract class FunctionInfoBase : ObjectInfo
	{
		/// <summary>
		/// Field name.
		/// </summary>
		public required string Name { get; init; }

		/// <summary>
		/// Return type.
		/// </summary>
		public required TypeInstanceInfo ReturnType { get; init; }

		/// <summary>
		/// Parameter types.
		/// </summary>
		public IList<TypeInstanceInfo> ArgumentTypes { get; init; } = new List<TypeInstanceInfo>();


		public string Signature => $"{ReturnType.Type.QualifiedName} {Name}({string.Join(",", ArgumentTypes.Select(x => x.Type.QualifiedName))})";
	}
}