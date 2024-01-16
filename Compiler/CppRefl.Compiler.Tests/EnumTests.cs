using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler.Tests
{
	internal class EnumTests : CompilerTestBase
	{
		protected override string ReflectionFile => Path.Combine(TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory.FullName, "EnumCode.h");

		[Test]
		public void EnumTypes()
		{
			TestUtil.TestType(Registry.GetType("ReflectedEnum")!, "ReflectedEnum", TypeKind.Enum, isEnum: true);
			TestUtil.TestType(Registry.GetType("TestNamespace::NamespacedEnum")!, "NamespacedEnum", TypeKind.Enum, isEnum: true, typeNamespace: "TestNamespace");

			Assert.That(Registry.GetEnum("ReflectedEnum")!.Type, Is.EqualTo(Registry.GetType("ReflectedEnum")));
			Assert.That(Registry.GetEnum("TestNamespace::NamespacedEnum")!.Type, Is.EqualTo(Registry.GetType("TestNamespace::NamespacedEnum")));
		}

		[Test]
		public void EnumValues()
		{
			{
				var enumInfo = Registry.GetEnum("ReflectedEnum")!;
				Assert.That(enumInfo.Values.Count, Is.EqualTo(3));

				Assert.That(enumInfo.Values[0].Name, Is.EqualTo("EnumValue1"));
				Assert.That(enumInfo.Values[0].Value, Is.EqualTo(0));

				Assert.That(enumInfo.Values[1].Name, Is.EqualTo("Bar"));
				Assert.That(enumInfo.Values[1].Value, Is.EqualTo(10));

				Assert.That(enumInfo.Values[2].Name, Is.EqualTo("Foo"));
				Assert.That(enumInfo.Values[2].Value, Is.EqualTo(11));
			}

			{
				var enumInfo = Registry.GetEnum("TestNamespace::NamespacedEnum")!;
				Assert.That(enumInfo.Values.Count, Is.EqualTo(2));

				Assert.That(enumInfo.Values[0].Name, Is.EqualTo("Valu1"));
				Assert.That(enumInfo.Values[0].Value, Is.EqualTo(0));

				Assert.That(enumInfo.Values[1].Name, Is.EqualTo("Valu2"));
				Assert.That(enumInfo.Values[1].Value, Is.EqualTo(1));
			}
		}
	}
}
