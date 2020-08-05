#include <stdio.h>
#include <string>

#include "ErrorHandling.h"
#include "Reflection.h"
#include "ReflectionGen.h"
#include "ReflectionUtil.h"

#include "RealTestCode.h"

static refl::Registry registry;

static void ErrorHandler(const char* message, const char* file, int line)
{
	printf("Error: %s\n", message);
	printf("%s (%d)\n", file, line);
	__debugbreak();
}

int main()
{
	refl::SetErrorHandler(ErrorHandler);

	refl::GenerationParameters params;
	params.mInputFilepath = "X:\\projects\\CppRefl\\CppRefl\\CppReflTest\\RealTestCode.cpp";
	params.mClangArgs.push_back("-std=c++1z");
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
	test.vectorOfInts.push_back(4);
	test.vectorOfInts.push_back(7);
	test.vectorOfInts.push_back(11);

	const refl::Enum& testEnumRefl = registry.GetEnum("testns::TestEnum");
	printf("\n%s\n", testEnumRefl.ToString().c_str());

	const refl::Class& testStructRefl = registry.GetClass("testns::TestStruct");
	printf("\n%s\n", testStructRefl.ToString().c_str());
	
	const refl::Function& fNothing = testStructRefl.GetFunction("FuncVoidNoParams");
	auto badRV = fNothing.Invoke<int>(test);
	if (badRV) {
		// Should not happen.
		printf("GOT A BAD RETURN VALUE. BAAAAAAAAAAAAAAAAADDDDDDDDDDDD.");
	}

	const refl::Function& fOneParam = testStructRefl.GetFunction("FuncVoidOneParam");
	int i = 420;
	fOneParam.Invoke(test, i);

	const refl::Function& fReturnValueParam = testStructRefl.GetFunction("FuncIntOneParam");
	auto rv = fReturnValueParam.Invoke<int>(test, test.nestedStruct);
	if (rv) {
		printf("GOT RETURN VALUE: %d\n", *rv);
	}

	const refl::Function& reflFunction = testStructRefl.GetFunction("FuncIntNoParams");
	auto rv2 = reflFunction.Invoke<int>(test);
	if (rv2) {
		printf("GOT RETURN VALUE: %d\n", *rv2);
	}

	const refl::Field& vectorField = testStructRefl.GetField("vectorOfInts");
	size_t elems = vectorField.GetArraySize((void*)&test);
	printf("vector has %zu elements", elems);

	fflush(stdout);
}