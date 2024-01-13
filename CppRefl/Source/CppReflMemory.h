#pragma once

namespace cpprefl
{
	class IMemoryAllocator
	{
	public:
		virtual void* Allocate(size_t numBytes) = 0;
		virtual void Free(void* memory) = 0;
	};

	void SetMemoryAllocator(IMemoryAllocator* allocator);
	IMemoryAllocator& GetMemoryAllocator();
}