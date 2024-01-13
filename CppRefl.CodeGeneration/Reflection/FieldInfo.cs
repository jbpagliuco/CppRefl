namespace CppRefl.CodeGeneration.Reflection
{
	public class FieldInfo : ObjectInfo
	{
		/// <summary>
		/// Field name.
		/// </summary>
		public required string Name { get; init; }

		/// <summary>
		/// Field type.
		/// </summary>
		public required TypeInstanceInfo TypeInstance { get; init; }

		public TypeInfo Type => TypeInstance.Type;
	}
}
