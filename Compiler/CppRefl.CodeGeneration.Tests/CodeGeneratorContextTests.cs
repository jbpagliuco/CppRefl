namespace CppRefl.CodeGeneration.Tests
{
	internal class CodeGeneratorContextTests
	{
		[TearDown]
		public void RestoreConsole()
		{
			Console.SetOut(new StreamWriter(Console.OpenStandardOutput()));
			Console.SetError(new StreamWriter(Console.OpenStandardError()));
		}

		[Test]
		public void ShouldRaiseWarning()
		{
			// Override stdout
			using StringWriter sw = new StringWriter();
			Console.SetError(sw);

			CodeGeneratorContext context = new();
			context.AddDiagnostic(DiagnosticLevel.Warning, "Warning #1...");
			context.AddDiagnostic(DiagnosticLevel.Warning, "Warning #2...");

			Assert.That(sw.ToString(), Is.EqualTo($"Warning #1...{Environment.NewLine}Warning #2...{Environment.NewLine}"));
		}

		[Test]
		public void ShouldRaiseError()
		{
			CodeGeneratorContext context = new();
			Assert.Throws(
				Is.TypeOf<Exception>()
					.And.Message.EqualTo("Error"),
				() => context.AddDiagnostic(DiagnosticLevel.Error, "Error"));
		}
	}
}