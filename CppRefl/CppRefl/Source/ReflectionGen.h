#pragma once

#include "Reflection.h"

namespace refl
{
	bool GenerateReflectionRegistry(refl::Registry& outRegistry, const char* inputFilepath, const char** clangArgs, int numClangArgs);
}