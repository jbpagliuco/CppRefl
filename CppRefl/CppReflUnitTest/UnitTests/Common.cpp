#include "Common.h"

#include <filesystem>

#include <gtest/gtest.h>

#include "ReflectionGen.h"

static void CppReflErrorHandler(refl::ErrorType errorType, const char* message, const char* file, int line)
{
	__debugbreak();
	EXPECT_TRUE(false);
	printf("CppRefl Error: %s (%d)\n", file, line);
	printf("\t%s\n\n", message);
}

static std::vector<std::string> GetClangArgs()
{
	std::vector<std::string> args;

	args.push_back("-std=c++1z");
	args.push_back("-fms-compatibility");
	args.push_back("-fms-compatibility-version=19.26");
	args.push_back("-fms-extensions");
	args.push_back("-Wall");
	args.push_back("-Wmicrosoft");
	args.push_back("-Wno-c++98-compat");
	args.push_back("-Wno-c++98-compat-pedantic");
	args.push_back("-Wno-newline-eof");

	return args;
}

static std::vector<std::string> GetClangCLArgs()
{
	std::vector<std::string> args;

	args.push_back("--driver-mode=cl");
	args.push_back("/std:c++17");
	args.push_back("-fms-compatibility");
	args.push_back("-fms-compatibility-version=19.26");
	args.push_back("-fms-extensions");
	args.push_back("-Wall");
	args.push_back("-Wmicrosoft");
	args.push_back("-Wno-c++98-compat");
	args.push_back("-Wno-c++98-compat-pedantic");
	args.push_back("-Wno-newline-eof");

	return args;
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
	params.mClangArgs = GetClangArgs();
	params.mIncludePaths.push_back(reflIncludePath.string());
	params.mProjectPath = projectPath.string();

	const bool success = refl::GenerateReflectionRegistry(registry, params);
	ASSERT_TRUE(success);
}