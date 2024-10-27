namespace CppRefl.Compiler.Reflection
{
	/// <summary>
	/// An instance of a type. Contains data about modifiers.
	/// </summary>
	public class TypeInstanceInfo
	{
		/// <summary>
		/// Type.
		/// </summary>
		public required TypeInfo Type { get; init; }

		/// <summary>
		/// Whether or not this type is const.
		/// </summary>
		public required bool Const { get; init; }
	}
}
