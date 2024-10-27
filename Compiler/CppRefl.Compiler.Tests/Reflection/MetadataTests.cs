using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.Tests.Reflection
{
    internal class MetadataTests : CompilerTestBase
    {
        protected override string ReflectionFile => Path.Combine(Const.Runtime.Tests.ReflectedHeadersDirectory.FullName, "MetadataCode.h");

        private void ValidateTag(MetadataValue tag, string expectedTag, MetadataValueLifetime expectedLifetime)
        {
            Assert.That(tag.Value, Is.EqualTo(expectedTag));
            Assert.That(tag.Lifetime, Is.EqualTo(expectedLifetime));
        }

        private void ValidateAttribute(MetadataValue attribute, string expectedValue, MetadataValueLifetime expectedLifetime)
        {
            Assert.That(attribute.Value, Is.EqualTo(expectedValue));
            Assert.That(attribute.Lifetime, Is.EqualTo(expectedLifetime));
        }

        [Test]
        public void ClassTags()
        {
            var classInfo = Registry.GetClass("ClassWithMetadata")!;

            Assert.That(classInfo.Metadata.Tags.Count, Is.EqualTo(3));
            ValidateTag(classInfo.Metadata.Tags[0], "TagNoSpaces", MetadataValueLifetime.Runtime);
            ValidateTag(classInfo.Metadata.Tags[1], "Tag Spaces", MetadataValueLifetime.Runtime);
            ValidateTag(classInfo.Metadata.Tags[2], "Compile Only Tag", MetadataValueLifetime.Compile);
        }

        [Test]
        public void ClassAttributes()
        {
            var classInfo = Registry.GetClass("ClassWithMetadata")!;

            Assert.That(classInfo.Metadata.Attributes.Count, Is.EqualTo(6));
            ValidateAttribute(classInfo.Metadata.Attributes["Attr"], "\"Value\"", MetadataValueLifetime.Runtime);
            ValidateAttribute(classInfo.Metadata.Attributes["Attr 2"], "\"Value 2\"", MetadataValueLifetime.Runtime);
            ValidateAttribute(classInfo.Metadata.Attributes["Attr Number"], "12", MetadataValueLifetime.Runtime);
            ValidateAttribute(classInfo.Metadata.Attributes["Attr Unquoted String"], "ClassWithNoMetadata", MetadataValueLifetime.Runtime);
            ValidateAttribute(classInfo.Metadata.Attributes["Attr Multiple"], "1, 2, 3", MetadataValueLifetime.Runtime);
            ValidateAttribute(classInfo.Metadata.Attributes["Compile Only Attr"], "\"Compile Only Value\"", MetadataValueLifetime.Compile);
        }

        [Test]
        public void FieldMetadata()
        {
            var classInfo = Registry.GetClass("ClassWithMetadata")!;
            var fieldInfo = classInfo.Fields[0];

            Assert.That(fieldInfo.Metadata.Tags.Count, Is.EqualTo(2));
            ValidateTag(fieldInfo.Metadata.Tags[0], "Field Tag", MetadataValueLifetime.Runtime);
            ValidateTag(fieldInfo.Metadata.Tags[1], "Compile Tag", MetadataValueLifetime.Compile);

            Assert.That(fieldInfo.Metadata.Attributes.Count, Is.EqualTo(2));
            ValidateAttribute(fieldInfo.Metadata.Attributes["Field Attr"], "\"Value\"", MetadataValueLifetime.Runtime);
            ValidateAttribute(fieldInfo.Metadata.Attributes["Compile Attr"], "\"Compile Value\"", MetadataValueLifetime.Compile);
        }

        [Test]
        public void MethodMetadata()
        {
            var classInfo = Registry.GetClass("ClassWithMetadata")!;
            var methodInfo = classInfo.Methods[0];

            Assert.That(methodInfo.Metadata.Tags.Count, Is.EqualTo(2));
            ValidateTag(methodInfo.Metadata.Tags[0], "Method Tag", MetadataValueLifetime.Runtime);
            ValidateTag(methodInfo.Metadata.Tags[1], "Compile Tag", MetadataValueLifetime.Compile);

            Assert.That(methodInfo.Metadata.Attributes.Count, Is.EqualTo(2));
            ValidateAttribute(methodInfo.Metadata.Attributes["Method Attr"], "\"Value\"", MetadataValueLifetime.Runtime);
            ValidateAttribute(methodInfo.Metadata.Attributes["Compile Attr"], "\"Compile Value\"", MetadataValueLifetime.Compile);
        }

        [Test]
        public void SourceLocation()
        {
            var classInfo = Registry.GetClass("ClassWithMetadata")!;
            Assert.That(classInfo.Metadata.SourceLocation.FileInfo.FullName, Is.EqualTo(ReflectionFile));
            Assert.That(classInfo.Metadata.SourceLocation.Line, Is.EqualTo(28));
            Assert.That(classInfo.GeneratedBodyLine, Is.EqualTo(30));

            var enumInfo = Registry.GetEnum("EnumWithMetadata")!;
            Assert.That(enumInfo.Metadata.SourceLocation.FileInfo.FullName, Is.EqualTo(ReflectionFile));
            Assert.That(enumInfo.Metadata.SourceLocation.Line, Is.EqualTo(47));
            Assert.That(enumInfo.GeneratedBodyLine, Is.EqualTo(49));
        }

        [Test]
        public void CommentTests()
        {
            // No comments
            {
                var classInfo = Registry.GetClass("ClassWithNoMetadata")!;
                Assert.IsNull(classInfo.Metadata.Comment);
            }

            // With comments
            {
                var classInfo = Registry.GetClass("ClassWithMetadata")!;
                Assert.That(classInfo.Metadata.Comment, Is.EqualTo("Class comment."));
                Assert.That(classInfo.Fields[0].Metadata.Comment, Is.EqualTo("Field comment."));
                Assert.That(classInfo.Methods[0].Metadata.Comment, Is.EqualTo("Multline function comment."));
            }

        }
    }
}