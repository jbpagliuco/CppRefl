using System.Text.Json.Serialization;

namespace CppRefl.CodeGeneration.Reflection
{
	public class SourceLocation
	{
		/// <summary>
		/// Filepath where this object was defined.
		/// </summary>
		private readonly string _filepath = string.Empty;
		public required string Filepath { get => _filepath; init => _filepath = Path.GetFullPath(value); }

		/// <summary>
		/// The line where this object was defined.
		/// </summary>
		public required uint Line { get; init; }

		[JsonIgnore]
		public string Filename => Path.GetFileName(Filepath);

		[JsonIgnore]
		public string FilenameNoExt => Path.GetFileNameWithoutExtension(Filepath);

		[JsonIgnore]
		public string IdeDiagnostic => $"{Filepath}({Line}):";
	}
}
