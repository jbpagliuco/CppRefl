#pragma once

// Internal use only
#define CPP_REFLECTION_ANNOTATION "cpp_refl"
#define CPP_REFLECTION_SEPARATOR ","

#if defined(CPP_REFL_BUILD_REFLECTION)

// Basic reflection markup. Only needs to be used if no other reflection markup is needed.
#define REFLECTED				__attribute__((annotate(CPP_REFLECTION_ANNOTATION)))

// Reflects a tag with no value.
#define REFL_TAG(tag)			__attribute__((annotate(CPP_REFLECTION_ANNOTATION CPP_REFLECTION_SEPARATOR tag)))

// Reflects a tag with an assigned value.
#define REFL_ATTR(tag, value)	__attribute__((annotate(CPP_REFLECTION_ANNOTATION CPP_REFLECTION_SEPARATOR tag CPP_REFLECTION_SEPARATOR value)))

#else

// Compiles to nothing when not building reflection.
#define REFLECTED
#define REFL_TAG(...)
#define REFL_ATTR(...)

#endif