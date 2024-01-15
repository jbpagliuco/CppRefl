using System.Text;

namespace CppRefl.CodeGeneration.CodeWriters
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

    /// <summary>
    /// Class for writing code to a file.
    /// </summary>
    public class CodeWriter : IDisposable
    {
        /// <summary>
        /// Name of the file we're writing to.
        /// </summary>
        public string Filename { get; }

        /// <summary>
        /// Our file writer.
        /// </summary>
        protected StreamWriter Writer { get; }

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

        /// <summary>
        /// Write some text to the file. If the text contains multiple lines, each new line will be properly indented, etc.
        /// </summary>
        /// <param name="text"></param>
        private void WriteInternal(string text)
        {
            StringBuilder sb = new();

            // The last thing we wrote was a newline, so let's start a new one.
            if (LastWrittenChar == '\n')
            {
                string indent = string.Concat(Enumerable.Repeat("\t", IndentCount));
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

        /// <summary>
        /// Indent all new lines.
        /// </summary>
        /// <param name="num"></param>
        public void BeginIndent(int num = 1)
        {
            IndentCount += num;
        }

        /// <summary>
        /// Stop indenting new lines.
        /// </summary>
        /// <param name="num"></param>
        public void EndIndent(int num = 1)
        {
            IndentCount -= num;
        }

        /// <summary>
        /// Convenience disposable for controlling indentation.
        /// </summary>
        /// <param name="num"></param>
        /// <returns></returns>
        public CodeWriterIndent WithIndent(int num = 1)
        {
            return new CodeWriterIndent(this, num);
        }

        /// <summary>
        /// Add a string that will be postfixed to all new lines.
        /// </summary>
        /// <param name="postfix"></param>
        public void AddPostfix(string postfix)
        {
            Postfix += postfix;
        }

        /// <summary>
        /// Remove an existing postfix.
        /// </summary>
        /// <param name="postfix"></param>
        /// <exception cref="Exception"></exception>
        public void RemovePostfix(string postfix)
        {
            if (!Postfix.EndsWith(postfix))
            {
                throw new Exception($"Postfix does not end with '{postfix}'.");
            }

            Postfix = Postfix.Substring(0, Postfix.Length - postfix.Length);
        }

        /// <summary>
        /// Convenience disposable for controlling postfixes.
        /// </summary>
        /// <param name="postfix"></param>
        /// <returns></returns>
        public CodeWriterPostfix WithPostfix(string postfix)
        {
            return new CodeWriterPostfix(this, postfix);
        }

        /// <summary>
        /// Write a code block. Automatically indents any new lines.
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
