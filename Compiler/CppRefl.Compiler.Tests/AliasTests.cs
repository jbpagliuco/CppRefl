using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler.Tests
{
    internal class AliasTests : CompilerTestBase
    {
        protected override string ReflectionFile => Path.Combine(TestUtil.Const.Runtime.Tests.ReflectedHeadersDirectory, "AliasCode.h");

        [Test]
        public void BuiltinAlias()
        {
            AliasInfo aliasInfo = Registry.GetAlias("IntAlias")!;
            Assert.IsNotNull(aliasInfo);
            Assert.True(aliasInfo.Type.IsInteger);
            Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("int")));
            Assert.IsNull(aliasInfo.AliasClass);
            Assert.IsNull(aliasInfo.AliasEnum);
        }

        [Test]
        public void ClassAlias()
        {
            AliasInfo aliasInfo = Registry.GetAlias("ClassAlias")!;
            Assert.IsNotNull(aliasInfo);
            Assert.True(aliasInfo.Type.IsClass);
            Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("ClassForAlias")));
            Assert.That(aliasInfo.AliasClass, Is.EqualTo(Registry.GetClass("ClassForAlias")));
            Assert.IsNull(aliasInfo.AliasEnum);
        }

        [Test]
        public void EnumAlias()
        {
            AliasInfo aliasInfo = Registry.GetAlias("EnumAlias")!;
            Assert.IsNotNull(aliasInfo);
            Assert.True(aliasInfo.Type.IsEnum);
            Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("EnumForAlias")));
            Assert.IsNull(aliasInfo.AliasClass);
            Assert.That(aliasInfo.AliasEnum, Is.EqualTo(Registry.GetEnum("EnumForAlias")));
        }

        [Test]
        public void AliasAlias()
        {
            AliasInfo aliasInfo = Registry.GetAlias("AliasAlias")!;
            Assert.IsNotNull(aliasInfo);
            Assert.True(aliasInfo.Type.IsEnum);
            Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("EnumForAlias")));
            Assert.That(aliasInfo.GetUnderlyingEnum(Registry), Is.EqualTo(Registry.GetEnum("EnumForAlias")));
        }

        [Test]
        public void TemplateClassAlias()
        {
            AliasInfo aliasInfo = Registry.GetAlias("TemplateAlias")!;
            Assert.IsNotNull(aliasInfo);
            Assert.True(aliasInfo.Type.IsClass);
            Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("TemplatedClassForAlias<int>")));
            Assert.That(aliasInfo.AliasClass, Is.EqualTo(Registry.GetClass("TemplatedClassForAlias<int>")));
        }

        [Test]
        public void TemplateClassAliasAlias()
        {
            // TODO: See AliasCode.h for details.
            //AliasInfo aliasInfo = Registry.GetAlias("TemplateAliasAlias")!;
            //Assert.IsNotNull(aliasInfo);
            //Assert.True(aliasInfo.Type.IsClass);
            //Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("TemplatedClassForAlias<int>")));
            //Assert.That(aliasInfo.GetUnderlyingClass(Registry), Is.EqualTo(Registry.GetClass("TemplatedClassForAlias<int>")));
        }

        [Test]
        public void TemplateClassSpecializationAlias()
        {
            AliasInfo aliasInfo = Registry.GetAlias("TemplateSpecializationAlias")!;
            Assert.IsNotNull(aliasInfo);
            Assert.True(aliasInfo.Type.IsClass);
            Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("TemplatedSpecializationForAlias")));
            Assert.That(aliasInfo.GetUnderlyingClass(Registry), Is.EqualTo(Registry.GetClass("TemplatedSpecializationForAlias")));
        }

        [Test]
        public void AliasOfNamespace()
        {
            AliasInfo aliasInfo = Registry.GetAlias("NamespacedAlias")!;
            Assert.IsNotNull(aliasInfo);
            Assert.True(aliasInfo.Type.IsClass);
            Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("TestNamespace::NamespacedClassForAlias")));
            Assert.That(aliasInfo.GetUnderlyingClass(Registry), Is.EqualTo(Registry.GetClass("TestNamespace::NamespacedClassForAlias")));
        }

        [Test]
        public void NamespacedAlias()
        {
            AliasInfo aliasInfo = Registry.GetAlias("TestNamespace::NamespacedAlias")!;
            Assert.IsNotNull(aliasInfo);
            Assert.True(aliasInfo.Type.IsClass);
            Assert.That(aliasInfo.AliasType, Is.EqualTo(Registry.GetType("TestNamespace::NamespacedClassForAlias")));
            Assert.That(aliasInfo.GetUnderlyingClass(Registry), Is.EqualTo(Registry.GetClass("TestNamespace::NamespacedClassForAlias")));
        }
    }
}
