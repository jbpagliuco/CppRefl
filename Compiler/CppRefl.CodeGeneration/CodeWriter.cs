using System.Text;

namespace CppRefl.CodeGeneration
{
	/// <summary>
	/// Helper class for indenting a block of code.
	/// </summary>
	public class CodeWriterIndent : IDisposable
	{
		private CodeWriter Writer { get; }
		private int Num { get; }

		public CodeWriterIndent(CodeWriter writer, int num)
		{
			Writer = writer;
			Writer.BeginIndent(num);
			Num = num;
		}

		public void Dispose()
		{
			Writer.EndIndent(Num);
		}
	}

	/// <summary>
	/// Helper class for adding a postfix to every line in a block of code.
	/// </summary>
	public class CodeWriterPostfix : IDisposable
	{
		private CodeWriter Writer { get; }
		private string Postfix { get; }

		public CodeWriterPostfix(CodeWriter writer, string postfix)
		{
			Writer = writer;
			Postfix = postfix;
			Writer.AddPostfix(postfix);
		}

		public void Dispose()
		{
			Writer.RemovePostfix(Postfix);
		}
	}

	/// <summary>
	/// Helper class for writing a block of code.
	/// </summary>
	public class CodeWriterCodeBlock : IDisposable
	{
		private CodeWriter Writer { get; }
		private string End { get; }

		public CodeWriterCodeBlock(CodeWriter writer, string block, string start, string end)
		{
			Writer = writer;
			End = end;

			Writer.WriteLine(block);
			Writer.WriteLine(start);
			Writer.BeginIndent();
		}

		public void Dispose()
		{
			Writer.EndIndent();
			Writer.WriteLine(End);
		}
	}


	public class CodeWriter : IDisposable
	{
		/// <summary>
		/// Name of the file we're writing to.
		/// </summary>
		public string Filename { get; }

		/// <summary>
		/// Our file writer.
		/// </summary>
		private StreamWriter Writer { get; }

		/// <summary>
		/// The number of indents each line should have.
		/// </summary>
		private int IndentCount { get; set; } = 0;

		/// <summary>
		/// A string to append to every line.
		/// </summary>
		private string Postfix { get; set; } = "";

		/// <summary>
		/// The last character we wrote.
		/// </summary>
		private char LastWrittenChar { get; set; } = '\0';

		public CodeWriter(string filename)
		{
			Filename = filename;

			Directory.CreateDirectory(Path.GetDirectoryName(Filename)!);
			Writer = new StreamWriter(filename);
		}
		
		public void Dispose()
		{
			Writer.Dispose();
		}

		private void WriteInternal(string text)
		{
			StringBuilder sb = new();

			// The last thing we wrote was a newline, so let's start a new one.
			if (LastWrittenChar == '\n')
			{
				string indent = String.Concat(Enumerable.Repeat("\t", IndentCount));
				sb.Append(indent);
			}

			sb.Append(text);

			// Put in our postfix.
			if (text != "")
			{
				LastWrittenChar = text.Last();
				if (LastWrittenChar == '\n')
				{
					sb.Length -= 1;
					sb.Append(Postfix);
					sb.Append('\n');
				}
			}

			Writer.Write(sb.ToString());
		}

		/// <summary>
		/// Write a piece of text.
		/// </summary>
		/// <param name="text"></param>
		private void WriteBodyInternal(string text)
		{
			text = text.Replace("\r\n", "\n");
			string[] lines = text.Split("\n");
			if (lines.Last() == "")
			{
				lines = lines.Take(lines.Length - 1).ToArray();
			}

			for (int i = 0; i < lines.Length; ++i)
			{
				WriteInternal(lines[i]);

				if (i < lines.Length - 1 || text.Last() == '\n')
				{
					WriteInternal("\n");
				}
			}
		}

		/// <summary>
		/// Write some text to the output file.
		/// </summary>
		/// <param name="text"></param>
		public void Write(string text)
		{
			WriteBodyInternal(text);
		}

		/// <summary>
		/// Write a line of text to the output file.
		/// </summary>
		/// <param name="text"></param>
		public void WriteLine(string text = "")
		{
			WriteBodyInternal(text + "\n");
		}

		public void BeginIndent(int num = 1)
		{
			IndentCount += num;
		}

		public void EndIndent(int num = 1)
		{
			IndentCount -= num;
		}

		public CodeWriterIndent Indent(int num = 1)
		{
			return new CodeWriterIndent(this, num);
		}

		public CodeWriterIndent WithIndent(int num = 1)
		{
			return new CodeWriterIndent(this, num);
		}

		public void AddPostfix(string postfix)
		{
			Postfix += postfix;
		}

		public void RemovePostfix(string postfix)
		{
			if (!Postfix.EndsWith(postfix))
			{
				throw new Exception($"Postfix does not end with '{postfix}'.");
			}

			Postfix = Postfix.Substring(0, Postfix.Length - postfix.Length);
		}

		public CodeWriterPostfix WithPostfix(string postfix)
		{
			return new CodeWriterPostfix(this, postfix);
		}

		/// <summary>
		/// Write a code block.
		/// </summary>
		/// <param name="text"></param>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <returns></returns>
		public CodeWriterCodeBlock WithCodeBlock(string text, string start = "{", string end = "}")
		{
			return new CodeWriterCodeBlock(this, text, start, end);
		}
	}
}
