using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.Tests.CodeGeneration
{
	internal class HashTests
	{
		/// <summary>
		/// Hash function that always produces a collision.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		private uint CollisionHashFunction(string name) => 0xDEADBEEF;

		/// <summary>
		/// Create a default registry.
		/// </summary>
		/// <returns></returns>
		private Registry CreateRegistry() => new() { HashFunction = CollisionHashFunction };

		[Test]
		public void ShouldThrowHashCollisionException()
		{
			var dict = new Dictionary<uint, string> { { 0xDEADBEEF, "str1" } };
			Assert.NotNull(Hash.FindHashCollision(dict, "str2", x => x, CollisionHashFunction, out _));
		}

		[Test]
		public void ShouldThrowHashCollisionExceptionRegistryTypes()
		{
			var reg = CreateRegistry();
			reg.AddType(TestHelpers.CreateTypeInfo("Type1"));

			Assert.Throws<TypeHashCollisionException>(() =>
			{
				reg.AddType(TestHelpers.CreateTypeInfo("Type2"));
			});
		}

		[Test]
		public void ShouldThrowHashCollisionExceptionFunctions()
		{
			var reg = CreateRegistry();
			reg.AddFunction(TestHelpers.CreateFunctionInfo("Function1"));

			Assert.Throws<FunctionHashCollisionException>(() =>
			{
				reg.AddFunction(TestHelpers.CreateFunctionInfo("Function2"));
			});
		}

		[Test]
		public void ShouldThrowHashCollisionExceptionFields()
		{
			var reg = CreateRegistry();
			var classInfo = new ClassInfo
			{
				Type = TestHelpers.CreateTypeInfo("Class1"),
				IsAbstract = false,
				Metadata = TestHelpers.CreateMetadataInfo(),
				ClassType = ClassType.Class,
				Fields = new List<FieldInfo>
				{
					new()
					{
						Metadata = TestHelpers.CreateMetadataInfo(),
						Name = "Field1",
						TypeInstance = TestHelpers.CreateTypeInstanceInfo("int")
					},
					new()
					{
						Metadata = TestHelpers.CreateMetadataInfo(),
						Name = "Field2",
						TypeInstance = TestHelpers.CreateTypeInstanceInfo("float")
					}
				}
			};

			Assert.Throws<FieldHashCollisionException>(() =>
			{
				reg.AddClass(classInfo);
			});
		}

		[Test]
		public void ShouldThrowHashCollisionExceptionMethods()
		{
			var reg = CreateRegistry();
			var classInfo = new ClassInfo
			{
				Type = TestHelpers.CreateTypeInfo("Class1"),
				IsAbstract = false,
				Metadata = TestHelpers.CreateMetadataInfo(),
				ClassType = ClassType.Class,
				Methods = new List<MethodInfo>
				{
					new()
					{
						Metadata = TestHelpers.CreateMetadataInfo(),
						Name = "Method1",
						ReturnType = TestHelpers.CreateTypeInstanceInfo("float")
					},
					new()
					{
						Metadata = TestHelpers.CreateMetadataInfo(),
						Name = "Method2",
						ReturnType = TestHelpers.CreateTypeInstanceInfo("void")
					}
				}
			};

			Assert.Throws<MethodHashCollisionException>(() =>
			{
				reg.AddClass(classInfo);
			});
		}
	}
}
