#pragma once

#include "Private/CppReflGeneratedCodeMacros.h"

// Internal use only
#define CPPREFL_INTERNAL_ANNOTATION "cpprefl"

#if CPPREFL_BUILD_REFLECTION()
// Basic reflection markup. Only needs to be used if no other reflection markup is needed.
#define REFLECTED				__attribute__((annotate(CPPREFL_INTERNAL_ANNOTATION)))

// Reflects a tag with no value.
#define REFL_TAG(tag)			__attribute__((annotate(CPPREFL_INTERNAL_ANNOTATION "," tag)))

// Reflects a tag with an assigned value.
#define REFL_ATTR(tag, value)	__attribute__((annotate(CPPREFL_INTERNAL_ANNOTATION "," tag "," CPPREFL_INTERNAL_TO_STR(value))))

#else

// Compiles to nothing when not building reflection.
#define REFLECTED
#define REFL_TAG(...)
#define REFL_ATTR(...)

#endif