#include "Common.h"

#include <filesystem>

#include <gtest/gtest.h>

#include "ReflectionGen.h"

static void CppReflErrorHandler(const char* message, const char* file, int line)
{
	EXPECT_TRUE(false);
	printf("CppRefl Error: %s (%d)\n", file, line);
	printf("\t%s\n\n", message);
}

void CreateRegistry(refl::Registry &registry)
{
	// Set custom error handler for CppRefl errors.
	refl::SetErrorHandler(CppReflErrorHandler);

	// Reflection entry point.
	const std::filesystem::path projectPath = std::filesystem::current_path();
	const std::filesystem::path entryPointPath = std::filesystem::current_path().append("ReflectionEntryPoint.cpp");
	const std::filesystem::path reflIncludePath = std::filesystem::current_path() / ".." / "CppRefl" / "Source";

	refl::GenerationParameters params;
	params.mInputFilepath = entryPointPath.string();
	params.mClangArgs.push_back("-std=c++1z");
	params.mClangArgs.push_back("-Wall");
	params.mClangArgs.push_back("-Wmicrosoft");
	params.mIncludePaths.push_back(reflIncludePath.string());
	params.mProjectPath = projectPath.string();

	const bool success = refl::GenerateReflectionRegistry(registry, params);
	ASSERT_TRUE(success);
}