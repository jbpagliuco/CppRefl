#pragma once

#include "Private/CppReflGeneratedCodeMacros.h"

#if CPPREFL_BUILD_REFLECTION()

// Internal use only
#define CPPREFL_INTERNAL_ANNOTATION "cpprefl"
#define CPPREFL_INTERNAL_META_ANNOTATION(lifetime, name, ...) __attribute__((annotate(CPPREFL_INTERNAL_ANNOTATION "-meta-" lifetime ":" name "," CPPREFL_INTERNAL_TO_STR(__VA_ARGS__))))

// Basic reflection markup. Only needs to be used if no other reflection markup is needed.
#define REFLECTED __attribute__((annotate(CPPREFL_INTERNAL_ANNOTATION)))

// Defines metadata accessible at both compile-time and runtime.
#define REFL_META_RUNTIME(name, ...) CPPREFL_INTERNAL_META_ANNOTATION("runtime", name, __VA_ARGS__)

// Defines metadata only accessible a compile-time.
#define REFL_META_COMPILE(name, ...) CPPREFL_INTERNAL_META_ANNOTATION("compile", name, __VA_ARGS__)

#else

// Compiles to nothing when not building reflection.
#define REFLECTED
#define REFL_META_RUNTIME(name, ...)
#define REFL_META_COMPILE(name, ...)

#endif