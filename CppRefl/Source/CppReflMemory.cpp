#include "CppReflMemory.h"

#include <cstdlib>

namespace cpprefl
{
	class DefaultMemoryAllocator : public IMemoryAllocator
	{
	public:
		void* Allocate(size_t numBytes) override
		{
			return std::malloc(numBytes);
		}

		void Free(void* memory) override
		{
			std::free(memory);
		}
	};

	DefaultMemoryAllocator s_DefaultMemoryAllocator;
	IMemoryAllocator* s_MemoryAllocator = &s_DefaultMemoryAllocator;

	void SetMemoryAllocator(IMemoryAllocator* allocator)
	{
		s_MemoryAllocator = allocator;
	}

	IMemoryAllocator& GetMemoryAllocator()
	{
		return *s_MemoryAllocator;
	}
}
