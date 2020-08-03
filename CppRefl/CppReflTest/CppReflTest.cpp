#include <stdio.h>
#include <string>

#include "Reflection.h"
#include "ReflectionGen.h"
#include "ReflectionUtil.h"

#include "RealTestCode.h"

static refl::Registry registry;

static void ErrorHandler(const char* message, const char* file, int line)
{
	printf("Error: %s\n", message);
	printf("%s (%d)\n", file, line);
}

int main()
{
	refl::SetErrorHandler(ErrorHandler);

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

	const refl::Function& reflFunction = testStructRefl.GetFunction("FuncIntOneParam");
	int i = 55;
	reflFunction.Invoke((void*)&test, &i, test.nestedStruct);

	const refl::Function& reflFunction2 = testStructRefl.GetFunction("FuncVoidOneParam");
	const int t = 0;
	reflFunction2.Invoke((void*)&test, &i, t);

	/*const std::string serialized = refl::util::Serialize(registry, testStructRefl, &test);
	printf("\n%s\n", serialized.c_str());*/

	fflush(stdout);
}