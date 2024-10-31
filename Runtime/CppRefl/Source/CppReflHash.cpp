#include "CppReflHash.h"

namespace cpprefl
{
	DebugStringHashMap sDebugStringHashMap;

	Name EnsureName(const char* string)
	{
		const Name name = Name(string);

#if CPPREFL_STORE_NAMES()
		sDebugStringHashMap.Insert(name, string);
#endif

		return name;
	}

	const char* GetNameDebugString(const Name& name)
	{
		return sDebugStringHashMap.Get(name);
	}
}
