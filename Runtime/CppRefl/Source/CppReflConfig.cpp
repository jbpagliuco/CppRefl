#include "CppReflConfig.h"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

namespace cpprefl
{
	void* IConfig::AllocateMemory(size_t numBytes)
	{
		return std::malloc(numBytes);
	}

	void IConfig::FreeMemory(void* memory)
	{
		std::free(memory);
	}

	void IConfig::Log(LogLevel level, const char* fmt, ...)
	{
		static const char* const levelStrings[] =
		{
			"Warning",
			"Error",
			"Fatal"
		};

		va_list args;
		va_start(args, fmt);
		{
			std::printf("CppRefl [%s]: ", levelStrings[static_cast<int>(level)]);
			std::vprintf(fmt, args);
			std::printf("\n");
		}
		va_end(args);
	}

	IConfig s_defaultConfig;
	IConfig* s_config = &s_defaultConfig;

	IConfig& IConfig::Get()
	{
		return *s_config;
	}

	void IConfig::Set(IConfig* config)
	{
		s_config = config;
	}
}
