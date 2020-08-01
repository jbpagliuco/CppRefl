#pragma once

#include "Reflection.h"

namespace refl
{
	struct GenerationParameters
	{
		// .CPP file used to compile reflection.
		std::string mInputFilepath;

		// [Optional] Command-line arguments to pass to clang.
		std::vector<std::string> mClangArgs;
		// [Optional] Include filepaths.
		std::vector<std::string> mIncludePaths;

		// [Optional] Project filepath (any symbols defined outside of this file path are ignored).
		std::string mProjectPath;
	};

	bool GenerateReflectionRegistry(refl::Registry& outRegistry, const GenerationParameters& params);
}