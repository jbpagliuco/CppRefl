#include <stdio.h>
#include <string>

#include "Reflection.h"
#include "ReflectionGen.h"
#include "ReflectionUtil.h"

#include "RealTestCode.h"

static refl::Registry registry;

int main()
{
	refl::GenerationParameters params;
	params.mInputFilepath = "X:\\projects\\CppRefl\\CppRefl\\CppReflTest\\RealTestCode.cpp";
	params.mClangArgs.push_back("-Wall");
	params.mClangArgs.push_back("-Wmicrosoft");
	params.mIncludePaths.push_back("X:\\projects\\CppRefl\\CppRefl\\CppRefl\\Source");
	params.mProjectPath = "X:\\projects\\CppRefl\\CppRefl\\CppReflTest";
	
	bool success = refl::GenerateReflectionRegistry(registry, params);
	if (!success) {
		printf("Failed to generate reflection for '%s'\n", params.mInputFilepath.c_str());
	}

	testns::TestStruct test;
	test.i = 123;
	test.f = 654.321f;
	test.e = testns::TestEnum::VAL2;
	test.typedefInt = (int)'t';
	test.string = "this is a string";
	test.nestedStruct.b = true;

	const refl::Enum& testEnumRefl = registry.GetEnum("testns::TestEnum");
	printf("\n%s\n", testEnumRefl.ToString().c_str());

	const refl::Class& testStructRefl = registry.GetClass("testns::TestStruct");
	printf("\n%s\n", testStructRefl.ToString().c_str());

	const refl::Function& reflFunction = testStructRefl.GetFunction("FuncIntNoParams");
	int i = 0;
	reflFunction.Invoke((void*)&test, i);

	/*const std::string serialized = refl::util::Serialize(registry, testStructRefl, &test);
	printf("\n%s\n", serialized.c_str());*/

	fflush(stdout);
}