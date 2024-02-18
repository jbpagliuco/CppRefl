#include "CppReflConfig.h"

#include <cstdlib>

namespace cpprefl
{
	class DefaultConfig : public IConfig
	{
	public:
		void* AllocateMemory(size_t numBytes) override
		{
			return std::malloc(numBytes);
		}

		void FreeMemory(void* memory) override
		{
			std::free(memory);
		}
	};

	DefaultConfig s_defaultConfig;
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
