#pragma once

#ifndef CPPREFL_STORE_NAMES
#define CPPREFL_STORE_NAMES() _DEBUG
#endif

#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
#define CPPREFL_CONCEPTS() 1
#else
#define CPPREFL_CONCEPTS() 0
#endif

namespace cpprefl
{
	// Class to configure CppRefl.
	class IConfig
	{
	public:
		static IConfig& Get();
		static void Set(IConfig* config);

		virtual ~IConfig() = default;

		virtual void* AllocateMemory(size_t numBytes) = 0;
		virtual void FreeMemory(void* memory) = 0;
	};
}