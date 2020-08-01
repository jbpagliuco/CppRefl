#include "Reflection.h"

namespace refl
{
	namespace util
	{
		// Serialize some reflected data to a custom format. (Eventually this should use XML, JSON, etc.)
		std::string Serialize(const Registry& reflRegistry, const Class& reflClass, void* data);
	}
}