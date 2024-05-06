#include "CppReflConfig.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

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

	void Log(LogLevel level, const char* fmt, va_list args)
	{
		static const char* const levelStrings[] =
		{
			"Warning",
			"Error",
			"Fatal"
		};

		std::printf("CppRefl [%s]: ", levelStrings[static_cast<int>(level)]);
		std::vprintf(fmt, args);
		std::printf("\n");
	}

#if CPPREFL_LOG()
	void IConfig::Log(LogLevel level, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		{
			Log(level, fmt, args);
		}
		va_end(args);
	}
#endif

	void IConfig::RaiseFatalError(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		{
			Log(LogLevel::Fatal, fmt, args);
		}
		va_end(args);

		std::abort();
	}

	IConfig s_defaultConfig;
	IConfig* s_config = &s_defaultConfig;

	IConfig& IConfig::Get()
	{
		return *s_config;
	}

	void IConfig::Set(IConfig& config)
	{
		s_config = &config;
	}
}
