using System.Diagnostics;
using System.Text.Json.Serialization;

namespace CppRefl.CodeGeneration.Reflection
{
	public enum ClassType
	{
		Struct,
		Class
	}

	/// <summary>
	/// Reflected class.
	/// </summary>
	[DebuggerDisplay("{NameMixinStatics.QualifiedName(Type)}")]
	public class ClassInfo : ObjectInfo
	{
		/// <summary>
		/// Type information.
		/// </summary>
		public required TypeInfo Type { get; set; }

		/// <summary>
		/// Reflected fields.
		/// </summary>
		public IList<FieldInfo> Fields { get; init; } = new List<FieldInfo>();

		/// <summary>
		/// Reflected methods.
		/// </summary>
		public IList<MethodInfo> Methods { get; init; } = new List<MethodInfo>();

		/// <summary>
		/// Line number where the GENERATED_REFLECTION_CODE() macro is defined in this class.
		/// </summary>
		public uint? GeneratedBodyLine { get; set; }

		/// <summary>
		/// Record/class type.
		/// </summary>
		public required ClassType ClassType { get; init; }

		/// <summary>
		/// List of base classes.
		/// </summary>
		public IList<ClassInfo> BaseClasses { get; init; } = new List<ClassInfo>();

		/// <summary>
		/// Returns true if this class if abstract or not.
		/// </summary>
		public required bool IsAbstract { get; init; }

		/// <summary>
		/// List of all base classes, recusively enumerating upwards.
		/// </summary>
		[JsonIgnore]
		public IEnumerable<ClassInfo> RecursiveBaseClasses => BaseClasses.Concat(BaseClasses.SelectMany(x => x.RecursiveBaseClasses));

		/// <summary>
		/// List of all base classes, recusively enumerating upwards.
		/// </summary>
		[JsonIgnore]
		public IEnumerable<ClassInfo> RecursiveClasses => new[] { this }.Concat(RecursiveBaseClasses);

		/// <summary>
		/// Class type (as would be used in C++ code), i.e. "class" or "struct".
		/// </summary>
		public string ClassTypeString => ClassType.ToString().ToLower();

		/// <summary>
		/// Forward declaration (without the surrounding namespace).
		/// </summary>
		private string ForwardDeclarationNoNamespace => Type.IsTemplated
			? (Type.Template!.IsGeneric
				? $"{Type.Template.DeclarationSignature} {ClassTypeString} {Type.Name};"
				: $"template <> {ClassTypeString} {Type.Name};")
			: $"{ClassTypeString} {Type.Name};";

		/// <summary>
		/// Forward declaration of this class.
		/// </summary>
		public string ForwardDeclaration => Type.IsInGlobalNamespace()
			? ForwardDeclarationNoNamespace
			: $"namespace {Type.Namespace} {{ {ForwardDeclarationNoNamespace} }}";

		public override string ToString() => Type.ToString();
	}
}
