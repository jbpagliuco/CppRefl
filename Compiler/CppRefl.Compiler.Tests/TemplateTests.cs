using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler.Tests
{
	internal class TemplateTests : CompilerTestBase
	{
		protected override string ReflectionFile => Path.Combine(TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory.FullName, "TemplateCode.h");

		[Test]
		public void TemplateClass()
	{
			var typeInfo = Registry.GetType("TestNamespace::TemplatedClass")!;
			Assert.IsNotNull(typeInfo);
			Assert.True(typeInfo.IsTemplated);
			Assert.False(typeInfo.Template!.IsSpecialized);
			Assert.That(typeInfo.Template.Arguments.Select(x => x.QualifiedName()), Is.EquivalentTo(new[] {"T"}));

			var classInfo = Registry.GetClass("TestNamespace::TemplatedClass")!;
			Assert.IsNotNull(classInfo);
			Assert.That(classInfo.Fields.Count, Is.EqualTo(1));
			Assert.That(classInfo.Fields[0].Type.Name, Is.EqualTo("T"));
		}

		[Test]
		public void NonTemplatedClass()
		{
			var typeInfo = Registry.GetType("NonTemplatedClass")!;
			Assert.IsNotNull(typeInfo);
			Assert.False(typeInfo.IsTemplated);
		}

		[Test]
		public void TemplatedClassSpecialization()
		{
			// Implicit specialization
			{
				var typeInfo = Registry.GetType("TestNamespace::TemplatedClass2<float>")!;
				Assert.IsNotNull(typeInfo);
				Assert.True(typeInfo.IsTemplated);
				Assert.True(typeInfo.Template!.IsSpecialized);
				Assert.That(typeInfo.Template.Arguments.Select(x => x.QualifiedName()), Is.EquivalentTo(new[] { "float" }));

				Assert.IsNotNull(Registry.GetClass("TestNamespace::TemplatedClass2<float>"));
			}

			// Explicit specialization
			{
				var typeInfo = Registry.GetType("TestNamespace::TemplatedClassSpecialization")!;
				Assert.IsNotNull(typeInfo);
				Assert.False(typeInfo.IsTemplated);
			}
		}

		[Test]
		public void TemplatedClassFields()
		{
			var classInfo = Registry.GetClass("TestNamespace::TemplatedClassSpecialization")!;
			Assert.IsNotNull(classInfo);
			Assert.That(classInfo.Fields.Count, Is.EqualTo(2));
			Assert.That(classInfo.Fields[1].Type.Kind, Is.EqualTo(TypeKind.Float));
		}

		[Test]
		public void TemplatedField()
		{
			var typeInfo = Registry.GetType("std::vector<int>")!;
			Assert.IsNotNull(typeInfo);
			Assert.True(typeInfo.IsTemplated);
			Assert.True(typeInfo.Template!.IsSpecialized);
			Assert.That(typeInfo.Template.Arguments.Select(x => x.QualifiedName()), Is.EquivalentTo(new[] { "int", "std::allocator<int>" }));
		}

		[Test]
		public void TemplatedFieldWithNamespace()
		{
			var typeInfo = Registry.GetType("std::vector<TestNamespace::NamespacedClass>")!;
			Assert.IsNotNull(typeInfo);
			Assert.True(typeInfo.IsTemplated);
			Assert.True(typeInfo.Template!.IsSpecialized);
			Assert.That(typeInfo.Template.Arguments.Select(x => x.QualifiedName()), Is.EquivalentTo(new[] { "TestNamespace::NamespacedClass", "std::allocator<TestNamespace::NamespacedClass>" }));
		}

		[Test]
		public void TemplatedFieldWithUsedNamespace()
		{
			var typeInfo = Registry.GetType("std::vector<TestNamespace2::NamespacedClass>")!;
			Assert.IsNotNull(typeInfo);
			Assert.True(typeInfo.IsTemplated);
			Assert.True(typeInfo.Template!.IsSpecialized);
			Assert.That(typeInfo.Template.Arguments.Select(x => x.QualifiedName()), Is.EquivalentTo(new[] { "TestNamespace2::NamespacedClass", "std::allocator<TestNamespace2::NamespacedClass>" }));
		}
	}
}