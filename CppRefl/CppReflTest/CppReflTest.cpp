#include <stdio.h>
#include <string>

#include "Reflection.h"
#include "ReflectionGen.h"

#include "RealTestCode.h"

refl::Registry registry;

void printClass(void* blob, const refl::Class& reflClass, std::string prefix = "")
{
	std::string classPrefix = reflClass.mName + "::";

	for (auto field : reflClass.mFields)
	{
		std::string fieldName = classPrefix + field.mName;
		if (prefix != "")
		{
			fieldName = prefix + fieldName;
		}

		if (field.mIsEnum)
		{
			int *enumValue = field.GetDataPtr<int>(blob);
			std::string enumString = registry.GetEnum(field.mEnumType).GetValueString(*enumValue);
			enumString = registry.GetEnum(field.mEnumType).mName + "::" + enumString;
			printf("%s = %s\n", fieldName.c_str(), enumString.c_str());
			continue;
		}

		if (field.mIsString)
		{
			std::string* str = field.GetString(blob);
			printf("%s = %s\n", fieldName.c_str(), str->c_str());
			continue;
		}

		if (field.mIsArray)
		{
		}

		switch (field.mType)
		{
		case refl::Type::BOOL:
			printf("%s = %s\n", fieldName.c_str(), field.GetDataPtr<bool>(blob) ? "true" : "false");
			break;

		case refl::Type::INT32:
			printf("%s = %d\n", fieldName.c_str(), *field.GetDataPtr<int>(blob));
			break;

		case refl::Type::FLOAT:
			printf("%s = %f\n", fieldName.c_str(), *field.GetDataPtr<float>(blob));
			break;

		case refl::Type::CLASS:
		{
			printClass(field.GetRawDataPtr(blob), registry.GetClass(field.mClassType), classPrefix);
			break;
		}

		};
	}
}

int main()
{
	const char* inputFile = "X:\\projects\\cpprefl\\CppReflTest\\CppReflTest\\RealTestCode.cpp";
	const char* clangArgs[] = { "-Wall", "-Wmicrosoft" };
	
	bool success = refl::GenerateReflectionRegistry(registry, inputFile, clangArgs, 2);
	if (!success) {
		printf("Failed to generate reflection for '%s'\n", inputFile);
	}

	TestStruct test;
	test.i = 123;
	test.f = 654.321f;
	test.e = TestEnum::VAL2;
	test.typedefInt = (int)'t';
	test.string = "this is a string";
	//test.nestedStruct.b = true;

	const refl::Class& testStructRefl = registry.GetClass("TestStruct");
	printf("%s", testStructRefl.ToString().c_str());

	printf("\n");

	const refl::Enum& testEnumRefl = registry.GetEnum("TestEnum");
	printf("%s", testEnumRefl.ToString().c_str());

	fflush(stdout);
}