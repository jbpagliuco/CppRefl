using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler.Tests
{
	internal class FunctionTests : CompilerTestBase
	{
		protected override string ReflectionFile => Path.Combine(Const.Runtime.Tests.ReflectedHeadersDirectory.FullName, "FunctionCode.h");

		[Test]
		public void FunctionInfo()
		{
			// ::ReflectedFunction
			{
				FunctionInfo func = Registry.GetFunction("ReflectedFunction")!;
				Assert.IsNotNull(func);
				Assert.That(func.Name, Is.EqualTo("ReflectedFunction"));
				Assert.That(func.Namespace, Is.EqualTo(""));
				Assert.That(func.ReturnType.Type, Is.EqualTo(Registry.GetType("void")!));
			}

			// TestNamespace::ReflectedFunction
			{
				FunctionInfo func = Registry.GetFunction("TestNamespace::ReflectedFunction")!;
				Assert.IsNotNull(func);
				Assert.That(func.Name, Is.EqualTo("ReflectedFunction"));
				Assert.That(func.Namespace, Is.EqualTo("TestNamespace"));
				Assert.That(func.ReturnType.Type, Is.EqualTo(Registry.GetType("void")!));
			}
		}
	}
}