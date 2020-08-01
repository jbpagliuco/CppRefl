#pragma once

#include "Reflection.h"

namespace refl
{
	bool GenerateReflectionRegistry(refl::Registry& outRegistry, const std::string& inputFilepath, const std::vector<std::string>& clangArgs, const std::vector<std::string>& includePaths);
}