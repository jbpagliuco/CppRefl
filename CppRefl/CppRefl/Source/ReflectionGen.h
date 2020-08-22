#pragma once

#include "Reflection.h"

namespace refl
{
	// List of compilers types.
	// NB: This is mainly used for reflecting classes in the std namespace.
	enum class CompilerType
	{
		INVALID = -1,
		CLANG = 0,
		MSVC
	};

	struct GenerationParameters
	{
		// .CPP file used to compile reflection.
		std::string mInputFilepath;

		// Target compiler (needed when reflected classes in the std namespace).
		CompilerType mTargetCompiler = CompilerType::INVALID;

		// [Optional] Command-line arguments to pass to clang.
		std::vector<std::string> mClangArgs;
		// [Optional] Include filepaths.
		std::vector<std::string> mIncludePaths;

		// [Optional] Project filepath (any symbols defined outside of this file path are ignored).
		std::string mProjectPath;

		// [Optional] Disables warnings about reflected classes in the std namespace.
		bool mDisableStdWarnings = false;

		// [Optional] Raise clang warnings.
		bool mRaiseClangWarnings = true;
		// [Optional] Raise clang errors.
		bool mRaiseClangErrors = true;
	};

	bool GenerateReflectionRegistry(refl::Registry& outRegistry, const GenerationParameters& params);
}