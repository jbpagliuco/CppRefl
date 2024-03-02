#pragma once

#ifndef CPPREFL_DEBUG
#define CPPREFL_DEBUG() _DEBUG
#endif

#ifndef CPPREFL_STORE_NAMES
#define CPPREFL_STORE_NAMES() CPPREFL_DEBUG()
#endif

#ifndef CPPREFL_LOG
#define CPPREFL_LOG() CPPREFL_DEBUG()
#define CPPREFL_INTERNAL_LOG(level, fmt, ...) IConfig::Get().Log(level, fmt, __VA_ARGS__)
#else
#define CPPREFL_INTERNAL_LOG(...)
#endif

#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
#define CPPREFL_CONCEPTS() 1
#else
#define CPPREFL_CONCEPTS() 0
#endif

namespace cpprefl
{
	enum class LogLevel
	{
		Warning = 0,
		Error,
		Fatal
	};

	// Class to configure CppRefl.
	class IConfig
	{
	public:
		static IConfig& Get();
		static void Set(IConfig* config);
		void Set() { Set(this); }

		virtual ~IConfig() = default;

		virtual void* AllocateMemory(size_t numBytes);
		virtual void FreeMemory(void* memory);

#if CPPREFL_LOG()
		virtual void Log(LogLevel level, const char* fmt, ...);
#endif
	};
}