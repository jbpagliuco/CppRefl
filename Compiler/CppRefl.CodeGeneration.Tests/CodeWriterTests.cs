using CppRefl.CodeGeneration.CodeWriters;

namespace CppRefl.CodeGeneration.Tests
{
	internal class CodeWriterTests
	{
		[Test]
		public void ShouldWriteText()
		{
			CodeWriter codeWriter = new();
			codeWriter.Write("This is some text.");
			codeWriter.Write(" This is some more text.");
			Assert.That(codeWriter.ToString(), Is.EqualTo("This is some text. This is some more text."));
		}

		[Test]
		public void ShouldWriteLine()
		{
			CodeWriter codeWriter = new();
			codeWriter.WriteLine("This is some text on a line.");
			codeWriter.WriteLine("Line #2.");
			Assert.That(codeWriter.ToString(), Is.EqualTo($"This is some text on a line.{Environment.NewLine}Line #2.{Environment.NewLine}"));
		}

		[Test]
		public void ShouldIndentText()
		{
			CodeWriter codeWriter = new();
			using (codeWriter.WithIndent())
			{
				codeWriter.Write("Line #1.\nLine #2.\n");
			}

			codeWriter.Write("Line #3.");

			Assert.That(codeWriter.ToString(), Is.EqualTo($"\tLine #1.{Environment.NewLine}\tLine #2.{Environment.NewLine}Line #3."));
		}

		[Test]
		public void ShouldIndentLines()
		{
			CodeWriter codeWriter = new();
			using (codeWriter.WithIndent())
			{
				codeWriter.WriteLine("Line #1.");
				codeWriter.WriteLine("Line #2.");
			}

			codeWriter.WriteLine("Line #3.");

			Assert.That(codeWriter.ToString(), Is.EqualTo($"\tLine #1.{Environment.NewLine}\tLine #2.{Environment.NewLine}Line #3.{Environment.NewLine}"));
		}

		[Test]
		public void ShouldPostfixText()
		{
			CodeWriter codeWriter = new();
			using (codeWriter.WithPostfix(";"))
			{
				codeWriter.Write("Line #1.\nLine #2.\n");
			}

			codeWriter.Write("Line #3.");

			Assert.That(codeWriter.ToString(), Is.EqualTo($"Line #1.;{Environment.NewLine}Line #2.;{Environment.NewLine}Line #3."));
		}

		[Test]
		public void ShouldPostfixLines()
		{
			CodeWriter codeWriter = new();
			using (codeWriter.WithPostfix(";"))
			{
				codeWriter.WriteLine("Line #1.");
				codeWriter.WriteLine("Line #2.");
			}

			codeWriter.WriteLine("Line #3.");

			Assert.That(codeWriter.ToString(), Is.EqualTo($"Line #1.;{Environment.NewLine}Line #2.;{Environment.NewLine}Line #3.{Environment.NewLine}"));
		}

		[Test]
		public void ShouldAddCodeBlock()
		{
			CodeWriter codeWriter = new();
			using (codeWriter.WithCodeBlock("CodeBlock", "{", "}"))
			{
				codeWriter.WriteLine("Line #1.");
				codeWriter.WriteLine("Line #2.");
			}

			codeWriter.WriteLine("Line #3.");

			string expected = """
			                  CodeBlock
			                  {
			                  	Line #1.
			                  	Line #2.
			                  }
			                  Line #3.
			                  """ + Environment.NewLine;
			Assert.That(codeWriter.ToString(), Is.EqualTo(expected));
		}
	}
}
