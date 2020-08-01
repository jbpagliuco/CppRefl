#include <stdio.h>
#include <string>

#include "Reflection.h"
#include "ReflectionGen.h"
#include "ReflectionUtil.h"

#include "RealTestCode.h"

static refl::Registry registry;

int main()
{
	const char* inputFile = "X:\\projects\\CppRefl\\CppRefl\\CppRefl\\CppReflTest\\RealTestCode.cpp";
	const char* clangArgs[] = { "-Wall", "-Wmicrosoft" };
	
	bool success = refl::GenerateReflectionRegistry(registry, inputFile, clangArgs, 2);
	if (!success) {
		printf("Failed to generate reflection for '%s'\n", inputFile);
	}

	testns::TestStruct test;
	test.i = 123;
	test.f = 654.321f;
	test.e = testns::TestEnum::VAL2;
	test.typedefInt = (int)'t';
	test.string = "this is a string";
	//test.nestedStruct.b = true;

	const refl::Class& testStructRefl = registry.GetClass("testns::TestStruct");
	const std::string serialized = refl::util::Serialize(registry, testStructRefl, &test);
	printf("\n%s\n", serialized.c_str());

	fflush(stdout);
}