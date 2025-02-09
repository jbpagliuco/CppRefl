#include "CppReflHash.h"

namespace cpprefl
{
#if CPPREFL_STORE_NAMES()
	DebugStringHashMap sDebugStringHashMap;

	Name EnsureName(const char* string)
	{
		const Name name = Name(string);

		sDebugStringHashMap.Insert(name, string)

		return name;
	}

	const char* GetNameDebugString(const Name& name)
	{
		return sDebugStringHashMap.Get(name);
	}
#endif
}
