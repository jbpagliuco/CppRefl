using CppRefl.Compiler.Reflection;

namespace CppRefl.Compiler.Tests.Reflection
{
    internal class ClassTests : CompilerTestBase
    {
        protected override string ReflectionFile => Path.Combine(Const.Runtime.Tests.ReflectedHeadersDirectory.FullName, "ClassCode.h");

        [Test]
        public void ClassType()
        {
            TestUtil.TestType(Registry.GetType("ReflectedClass")!, "ReflectedClass", TypeKind.Class, isClass: true);
            Assert.That(Registry.GetClass("ReflectedClass")!.Type, Is.EqualTo(Registry.GetType("ReflectedClass")));

            TestUtil.TestType(Registry.GetType("TestNamespace::ReflectedClass")!, "ReflectedClass", TypeKind.Class, isClass: true, typeNamespace: "TestNamespace");
            Assert.That(Registry.GetClass("TestNamespace::ReflectedClass")!.Type, Is.EqualTo(Registry.GetType("TestNamespace::ReflectedClass")));

            Assert.IsNotNull(Registry.GetClass("ReflectedStruct"));
            Assert.IsNull(Registry.GetClass("NonReflectedClass"));
        }

        [Test]
        public void ClassFields()
        {
            var classInfo = Registry.GetClass("ReflectedClass")!;
            Assert.That(classInfo.Fields.Count, Is.EqualTo(6));

            int fieldIndex = 0;

            Assert.That(classInfo.Fields[fieldIndex].Name, Is.EqualTo("mPublicInt"));
            TestUtil.TestTypeInstanceInfo(classInfo.Fields[fieldIndex].TypeInstance, Registry.GetType("int")!);
            ++fieldIndex;

            Assert.That(classInfo.Fields[fieldIndex].Name, Is.EqualTo("mConstInt"));
            TestUtil.TestTypeInstanceInfo(classInfo.Fields[fieldIndex].TypeInstance, Registry.GetType("int")!, isConst: true);
            ++fieldIndex;

            Assert.That(classInfo.Fields[fieldIndex].Name, Is.EqualTo("mStruct1"));
            TestUtil.TestTypeInstanceInfo(classInfo.Fields[fieldIndex].TypeInstance, Registry.GetType("ReflectedStruct")!);
            ++fieldIndex;

            Assert.That(classInfo.Fields[fieldIndex].Name, Is.EqualTo("mStruct2"));
            TestUtil.TestTypeInstanceInfo(classInfo.Fields[fieldIndex].TypeInstance, Registry.GetType("ReflectedStruct2")!);
            ++fieldIndex;

            Assert.That(classInfo.Fields[fieldIndex].Name, Is.EqualTo("mProtectedFloat"));
            TestUtil.TestTypeInstanceInfo(classInfo.Fields[fieldIndex].TypeInstance, Registry.GetType("float")!);
            ++fieldIndex;

            Assert.That(classInfo.Fields[fieldIndex].Name, Is.EqualTo("mPrivateChar"));
            TestUtil.TestTypeInstanceInfo(classInfo.Fields[fieldIndex].TypeInstance, Registry.GetType("char")!);
            ++fieldIndex;
        }

        [Test]
        public void ClassMethods()
        {
            var classInfo = Registry.GetClass("ReflectedClass")!;
            Assert.That(classInfo.Methods.Count, Is.EqualTo(4));

            int methodIndex = 0;

            // void FuncVoidNoParam()
            Assert.That(classInfo.Methods[methodIndex].Name, Is.EqualTo("FuncVoidNoParam"));
            TestUtil.TestTypeInstanceInfo(classInfo.Methods[methodIndex].ReturnType, Registry.GetType("void")!);
            Assert.IsEmpty(classInfo.Methods[methodIndex].ArgumentTypes);
            ++methodIndex;

            // void FuncVoidOneParam(int param1)
            Assert.That(classInfo.Methods[methodIndex].Name, Is.EqualTo("FuncVoidOneParam"));
            TestUtil.TestTypeInstanceInfo(classInfo.Methods[methodIndex].ReturnType, Registry.GetType("void")!);
            Assert.That(classInfo.Methods[methodIndex].ArgumentTypes.Count, Is.EqualTo(1));
            TestUtil.TestTypeInstanceInfo(classInfo.Methods[methodIndex].ArgumentTypes[0], Registry.GetType("int")!);
            ++methodIndex;

            // double FuncDoubleNoParam()
            Assert.That(classInfo.Methods[methodIndex].Name, Is.EqualTo("FuncDoubleNoParam"));
            TestUtil.TestTypeInstanceInfo(classInfo.Methods[methodIndex].ReturnType, Registry.GetType("double")!);
            Assert.IsEmpty(classInfo.Methods[methodIndex].ArgumentTypes);
            ++methodIndex;

            // double FuncDoubleTwoParam(int param1, short param2)
            Assert.That(classInfo.Methods[methodIndex].Name, Is.EqualTo("FuncDoubleTwoParam"));
            TestUtil.TestTypeInstanceInfo(classInfo.Methods[methodIndex].ReturnType, Registry.GetType("double")!);
            Assert.That(classInfo.Methods[methodIndex].ArgumentTypes.Count, Is.EqualTo(2));
            TestUtil.TestTypeInstanceInfo(classInfo.Methods[methodIndex].ArgumentTypes[0], Registry.GetType("int")!);
            TestUtil.TestTypeInstanceInfo(classInfo.Methods[methodIndex].ArgumentTypes[1], Registry.GetType("short")!);
            ++methodIndex;
        }
    }
}