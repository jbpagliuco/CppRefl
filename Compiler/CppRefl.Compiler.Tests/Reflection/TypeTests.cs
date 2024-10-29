using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.Tests.Reflection
{
	internal class TypeTests : CompilerTestBase
	{
		protected override string ReflectionFile => Path.Combine(Const.Runtime.Tests.ReflectedHeadersDirectory.FullName, "TypesCode.h");

		[Test]
		public void PrimitiveTypes()
		{
			TestUtil.TestType(Registry.GetType("bool")!, "bool", TypeKind.Bool, isPrimitive: true);

			TestUtil.TestType(Registry.GetType("unsigned char")!, "unsigned char", TypeKind.Uint8, isPrimitive: true, isInteger: true, isUnsigned: true);
			TestUtil.TestType(Registry.GetType("char")!, "char", TypeKind.Int8, isPrimitive: true, isInteger: true, isSigned: true);

			TestUtil.TestType(Registry.GetType("unsigned short")!, "unsigned short", TypeKind.Uint16, isPrimitive: true, isInteger: true, isUnsigned: true);
			TestUtil.TestType(Registry.GetType("short")!, "short", TypeKind.Int16, isPrimitive: true, isInteger: true, isSigned: true);

			TestUtil.TestType(Registry.GetType("unsigned int")!, "unsigned int", TypeKind.Uint32, isPrimitive: true, isInteger: true, isUnsigned: true);
			TestUtil.TestType(Registry.GetType("int")!, "int", TypeKind.Int32, isPrimitive: true, isInteger: true, isSigned: true);

			TestUtil.TestType(Registry.GetType("unsigned long")!, "unsigned long", TypeKind.Uint32, isPrimitive: true, isInteger: true, isUnsigned: true);
			TestUtil.TestType(Registry.GetType("long")!, "long", TypeKind.Int32, isPrimitive: true, isInteger: true, isSigned: true);

			TestUtil.TestType(Registry.GetType("unsigned long long")!, "unsigned long long", TypeKind.Uint64, isPrimitive: true, isInteger: true, isUnsigned: true);
			TestUtil.TestType(Registry.GetType("long long")!, "long long", TypeKind.Int64, isPrimitive: true, isInteger: true, isSigned: true);

			TestUtil.TestType(Registry.GetType("float")!, "float", TypeKind.Float, isPrimitive: true, isReal: true);
			TestUtil.TestType(Registry.GetType("double")!, "double", TypeKind.Double, isPrimitive: true, isReal: true);
			TestUtil.TestType(Registry.GetType("long double")!, "long double", TypeKind.LongDouble, isPrimitive: true, isReal: true);
		}
	}
}
