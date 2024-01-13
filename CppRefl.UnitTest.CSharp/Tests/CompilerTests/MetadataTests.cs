using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.UnitTest.CSharp.Tests.CompilerTests
{
	internal class MetadataTests : CompilerTestBase
	{
		protected override string ReflectionFile => Path.Combine(TestUtil.Filepaths.ReflectedCodeDirectory, "MetadataCode.h");

		[Test]
		public void MetadataTest()
		{
			var classInfo = Registry.GetClass("ClassWithAttributes")!;

			Assert.That(classInfo.Metadata.Tags.Count, Is.EqualTo(2));
			Assert.That(classInfo.Metadata.Tags[0], Is.EqualTo("SomeTag"));
			Assert.That(classInfo.Metadata.Tags[1], Is.EqualTo("Another Tag"));
			Assert.That(classInfo.Metadata.SourceLocation.Filepath, Is.EqualTo(ReflectionFile));
			Assert.That(classInfo.Metadata.SourceLocation.Filename, Is.EqualTo("MetadataCode.h"));
			Assert.That(classInfo.Metadata.SourceLocation.Line, Is.EqualTo(13));
			Assert.That(classInfo.GeneratedBodyLine, Is.EqualTo(15));

			Assert.That(classInfo.Metadata.Attributes.Count, Is.EqualTo(2));
			Assert.That(classInfo.Metadata.Attributes["SomeAttr"], Is.EqualTo("\"MyValue\""));
			Assert.That(classInfo.Metadata.Attributes["Second Attr"], Is.EqualTo("12"));

			var fieldInfo = classInfo.Fields[0];
			Assert.That(fieldInfo.Metadata.Tags.Count, Is.EqualTo(1));
			Assert.That(fieldInfo.Metadata.Tags[0], Is.EqualTo("FieldTag"));
			Assert.That(fieldInfo.Metadata.Attributes.Count, Is.EqualTo(2));
			Assert.That(fieldInfo.Metadata.Attributes["FieldAttr"], Is.EqualTo("\"Value\""));

			var methodInfo = classInfo.Methods[0];
			Assert.That(methodInfo.Metadata.Tags.Count, Is.EqualTo(1));
			Assert.That(methodInfo.Metadata.Tags[0], Is.EqualTo("FunctionTag"));
			Assert.That(methodInfo.Metadata.Attributes.Count, Is.EqualTo(1));
			Assert.That(methodInfo.Metadata.Attributes["FunctionAttr"], Is.EqualTo("\"Function\""));
		}
	}
}