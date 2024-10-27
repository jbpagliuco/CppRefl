namespace CppRefl.CodeGeneration.Tests
{
	internal class ModuleCodeGeneratorContextTests
	{
		private string ExpectedHeaderPrefix =>
			"""
			 #pragma once
			 
			 // Register the reflection runtime information for this module.
			 void RegisterModuleNameReflection();
			 """;

		private string ExpectedSourcePrefix =>
			"""
			#include "ModuleName.reflgen.h"
			
			#include "CppReflStatics.h"
			
			""";

		/// <summary>
		/// Creates a default code generator context.
		/// </summary>
		/// <returns></returns>
		private static ModuleCodeGeneratorContext CreateCodeGeneratorContext()
		{
			var root = Path.GetTempPath();
			return new()
			{
				Parameters = new()
				{
					ModuleName = "ModuleName",
					Registry = new(),
					ModuleDirectory = new DirectoryInfo($@"{root}Source"),
					OutputDirectory = new DirectoryInfo($@"{root}Generated")
				}
			};
		}

		[Test]
		public void ShouldWriteInitializerFunction()
		{
			var context = CreateCodeGeneratorContext();

			context.WriteInitializer(writer =>
			{
				writer.WriteLine("Initializer body.");
			});

			Assert.That(context.DumpHeader().Trim(), Is.EqualTo(ExpectedHeaderPrefix.Trim()));
			Assert.That(context.DumpSource().Trim(), Is.EqualTo(
				$$"""
				{{ExpectedSourcePrefix}}
				
				
				void RegisterModuleNameReflection()
				{
					Initializer body.
				
				}
				"""));
		}

		[Test]
		public void ShouldWriteHeader()
		{
			var context = CreateCodeGeneratorContext();

			context.WriteHeader(writer =>
			{
				writer.WriteLine("Header Line #1.");
				writer.WriteLine("Header Line #2.");
			});

			Assert.That(context.DumpHeader().Trim(), Is.EqualTo(
				$"""
				{ExpectedHeaderPrefix}
				
				Header Line #1.
				Header Line #2.
				"""));
			Assert.That(context.DumpSource().Trim(), Is.EqualTo(
				$$"""
				  {{ExpectedSourcePrefix}}

				  
				  void RegisterModuleNameReflection()
				  {
				  
				  }
				  """));
		}

		[Test]
		public void ShouldWriteSource()
		{
			var context = CreateCodeGeneratorContext();
			context.WriteSource(writer =>
			{
				writer.WriteLine("Source Line #1.");
				writer.WriteLine("Source Line #2.");
			});

			Assert.That(context.DumpHeader().Trim(), Is.EqualTo(ExpectedHeaderPrefix.Trim()));
			Assert.That(context.DumpSource().Trim(), Is.EqualTo(
				$$"""
				  {{ExpectedSourcePrefix}}
				  Source Line #1.
				  Source Line #2.
				  
				  
				  void RegisterModuleNameReflection()
				  {
				  
				  }
				  """));
		}
	}
}
