namespace CppRefl.Compiler.Reflection
{
	public class FunctionInfo : FunctionInfoBase, INameMixin
	{
		/// <summary>
		/// Function namespace.
		/// </summary>
		public required string Namespace { get; init; }

		public override string ToString() => this.QualifiedName();
	}
}