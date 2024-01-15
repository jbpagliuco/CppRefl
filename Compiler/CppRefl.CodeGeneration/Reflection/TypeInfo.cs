using System.Diagnostics;
using System.Security.AccessControl;
using System.Text.Json.Serialization;

namespace CppRefl.CodeGeneration.Reflection
{
	public enum TypeKind
	{
		Invalid = 0,

		Bool = 1,

		Uint8,
		Int8,
		Uint16,
		Int16,
		Uint32,
		Int32,
		Uint64,
		Int64,

		Float,
		Double,
		LongDouble,

		Enum,
		Class, // struct, class, record, whatever

		Void, // void return type

		Template, // unknown template type
	}

	/// <summary>
	/// Info about a templated type.
	/// </summary>
	public class TemplateInfo
	{
		/// <summary>
		/// List of all template arguments.
		/// </summary>
		public IList<TypeInfo> Arguments { get; init; } = new List<TypeInfo>();

		/// <summary>
		/// Is this template specialized?
		/// </summary>
		public required bool IsSpecialized { get; init; }

		/// <summary>
		/// Is this template generic?
		/// </summary>
		public bool IsGeneric => !IsSpecialized;

		/// <summary>
		/// Declaration signature, e.g. template <typename T, typename U, ...>
		/// </summary>
		public string DeclarationSignature => $"template <{string.Join(",", Arguments.Select(x => $"typename {x}"))}>";

		/// <summary>
		/// Instantiation signature, e.g. <T,U,...>
		/// </summary>
		public string InstantiationSignature => $"<{string.Join(",", Arguments)}>";
	}

	/// <summary>
	/// A type declaration. Could be a class, primitive, enum, etc.
	/// </summary>
	[DebuggerDisplay("{QualifiedName}")]
	public class TypeInfo
	{
		/// <summary>
		/// Type name (without the namespace).
		/// </summary>
		public required string Name { get; init; }

		/// <summary>
		/// Type namespace.
		/// </summary>
		public required string Namespace { get; init; }
		
		/// <summary>
		/// Kind of type data.
		/// </summary>
		public TypeKind Kind { get; set; }

		/// <summary>
		/// Template information.
		/// </summary>
		public TemplateInfo? Template { get; set; }

		/// <summary>
		/// An enumerable of all nested namespaces.
		/// </summary>
		[JsonIgnore]
		public IEnumerable<string> Namespaces => Namespace.Split("::");

		/// <summary>
		/// Returns true if this type was defined in the global namespace.
		/// </summary>
		[JsonIgnore]
		public bool IsInGlobalNamespace => Namespace.Length == 0;

		/// <summary>
		/// Returns the qualified name of this type, excluding the global namespace.
		/// </summary>
		[JsonIgnore]
		public string QualifiedName => IsInGlobalNamespace ? Name : $"{Namespace}::{Name}";

		/// <summary>
		/// Returns the qualified name of this type, including the global namespace.
		/// </summary>
		[JsonIgnore]
		public string GloballyQualifiedName => IsInGlobalNamespace ? $"::{Name}" : $"::{Namespace}::{Name}";

		/// <summary>
		/// Returns a fully qualified name using underscores instead of colons.
		/// </summary>
		[JsonIgnore]
		public string FlattenedName => QualifiedName.Replace("::", "_");


		/// <summary>
		/// Returns true if this is a primitive data type.
		/// </summary>
		[JsonIgnore]
		public bool IsPrimitive => Kind >= TypeKind.Bool && Kind <= TypeKind.LongDouble;

		/// <summary>
		/// Return true if this is an integer data type.
		/// </summary>
		[JsonIgnore]
		public bool IsInteger => Kind >= TypeKind.Uint8 && Kind <= TypeKind.Int64;
		
		/// <summary>
		/// Return true if this is a real number data type.
		/// </summary>
		[JsonIgnore]
		public bool IsReal => Kind >= TypeKind.Float && Kind <= TypeKind.LongDouble;
		
		/// <summary>
		/// Return true if this is an unsigned integer type.
		/// </summary>
		[JsonIgnore]
		public bool IsUnsigned => new[] { TypeKind.Uint8, TypeKind.Uint16, TypeKind.Uint32, TypeKind.Uint64 }.Contains(Kind);

		/// <summary>
		/// Return true if this is an signed integer type.
		/// </summary>
		[JsonIgnore]
		public bool IsSigned => new[] { TypeKind.Int8, TypeKind.Int16, TypeKind.Int32, TypeKind.Int64 }.Contains(Kind);

		/// <summary>
		/// Is this an enum type?
		/// </summary>
		[JsonIgnore]
		public bool IsEnum => Kind == TypeKind.Enum;

		/// <summary>
		/// Is this a class type?
		/// </summary>
		[JsonIgnore]
		public bool IsClass => Kind == TypeKind.Class;

		/// <summary>
		/// Is this type templated?
		/// </summary>
		[JsonIgnore]
		public bool IsTemplated => Template != null;

		/// <summary>
		/// Name of the underlying template generic.
		/// </summary>
		[JsonIgnore]
		public string? TemplateType => IsTemplated ? QualifiedName.Substring(0, QualifiedName.IndexOf('<')) : null;

		/// <summary>
		/// Is this type instantiable?
		/// </summary>
		[JsonIgnore]
		public bool IsInstantiable => !IsTemplated || Template?.IsSpecialized == true;
		

		public override string ToString() => QualifiedName;
	}
}
