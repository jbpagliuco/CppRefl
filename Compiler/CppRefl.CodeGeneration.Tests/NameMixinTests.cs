using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.CodeGeneration.Tests
{
	internal class NameMixinInst : INameMixin
	{
		public required string Name { get; init; }
		public required string Namespace { get; init; }
	}

	internal class NameMixinTests
	{
		[Test]
		public void GlobalNamespace()
		{
			var mixin = new NameMixinInst()
			{
				Name = "MyClass",
				Namespace = ""
			};

			Assert.IsEmpty(mixin.Namespaces());
			Assert.True(mixin.IsInGlobalNamespace());
			Assert.That(mixin.QualifiedName(), Is.EqualTo("MyClass"));
			Assert.That(mixin.GloballyQualifiedName(), Is.EqualTo("::MyClass"));
			Assert.That(mixin.FlattenedName(), Is.EqualTo("MyClass"));
		}

		[Test]
		public void OneNamespace()
		{
			var mixin = new NameMixinInst()
			{
				Name = "string",
				Namespace = "std"
			};

			Assert.That(mixin.Namespaces(), Is.EquivalentTo(new[] { "std" }));
			Assert.False(mixin.IsInGlobalNamespace());
			Assert.That(mixin.QualifiedName(), Is.EqualTo("std::string"));
			Assert.That(mixin.GloballyQualifiedName(), Is.EqualTo("::std::string"));
			Assert.That(mixin.FlattenedName(), Is.EqualTo("std_string"));
		}

		[Test]
		public void TwoNamespaces()
		{
			var mixin = new NameMixinInst()
			{
				Name = "string",
				Namespace = "cpprefl::private"
			};

			Assert.That(mixin.Namespaces(), Is.EquivalentTo(new[] { "cpprefl", "private" }));
			Assert.False(mixin.IsInGlobalNamespace());
			Assert.That(mixin.QualifiedName(), Is.EqualTo("cpprefl::private::string"));
			Assert.That(mixin.GloballyQualifiedName(), Is.EqualTo("::cpprefl::private::string"));
			Assert.That(mixin.FlattenedName(), Is.EqualTo("cpprefl_private_string"));
		}

		[Test]
		public void NestedClass()
		{
			var mixin = new NameMixinInst()
			{
				Name = "OuterClass::InnerClass",
				Namespace = "cpprefl::private"
			};

			Assert.That(mixin.Namespaces(), Is.EquivalentTo(new[] { "cpprefl", "private" }));
			Assert.False(mixin.IsInGlobalNamespace());
			Assert.That(mixin.QualifiedName(), Is.EqualTo("cpprefl::private::OuterClass::InnerClass"));
			Assert.That(mixin.GloballyQualifiedName(), Is.EqualTo("::cpprefl::private::OuterClass::InnerClass"));
			Assert.That(mixin.FlattenedName(), Is.EqualTo("cpprefl_private_OuterClass_InnerClass"));
		}

		[Test]
		public void NestedClassGlobalNamespace()
		{
			var mixin = new NameMixinInst()
			{
				Name = "OuterClass::InnerClass",
				Namespace = ""
			};

			Assert.IsEmpty(mixin.Namespaces());
			Assert.True(mixin.IsInGlobalNamespace());
			Assert.That(mixin.QualifiedName(), Is.EqualTo("OuterClass::InnerClass"));
			Assert.That(mixin.GloballyQualifiedName(), Is.EqualTo("::OuterClass::InnerClass"));
			Assert.That(mixin.FlattenedName(), Is.EqualTo("OuterClass_InnerClass"));
		}
	}
}
