using System.Text.Json.Serialization;

namespace CppRefl.CodeGeneration.Reflection
{
	public class SourceLocation
	{
		[JsonConstructor]
		public SourceLocation(string filename, uint line)
		{
			FileInfo = new(filename);
			Line = line;
		}

		public SourceLocation(FileInfo fileInfo, uint line)
		{
			FileInfo = fileInfo;
			Line = line;
		}

		/// <summary>
		/// FileInfo where this object was defined.
		/// </summary>
		[JsonIgnore]
		public FileInfo FileInfo { get; init; }

		/// <summary>
		/// The line where this object was defined.
		/// </summary>
		public uint Line { get; }

		public string Filename
		{
			get => FileInfo.FullName;
			init => FileInfo = new FileInfo(value);
		}

		[JsonIgnore]
		public string FilenameNoExt => Path.GetFileNameWithoutExtension(FileInfo.FullName);

		[JsonIgnore]
		public string IdeDiagnostic => $"{FileInfo.FullName}({Line}):";
	}
}
