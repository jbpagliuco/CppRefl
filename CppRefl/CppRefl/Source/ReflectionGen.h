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

		// [Optional] Disables warnings about reflected classes in the std namespace.
		bool mDisableStdWarnings = false;
		// [Optional] Sizes of std classes. Required if reflecting anything in the std namespace.
		std::map<std::string, size_t> mStdClassSizes;

		// [Optional] Raise clang warnings.
		bool mRaiseClangWarnings = true;
		// [Optional] Raise clang errors.
		bool mRaiseClangErrors = true;
	};

	bool GenerateReflectionRegistry(refl::Registry& outRegistry, const GenerationParameters& params);
}