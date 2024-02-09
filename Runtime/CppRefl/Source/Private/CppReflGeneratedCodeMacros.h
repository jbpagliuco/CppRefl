// Use a header guard for the other macros in the file.
// We can't use #pragma once because we need to be able to redefine the GENERATED_REFLECTION_CODE() macro as needed.
#if !defined(CPPREFL_GENERATED_CODE_MACROS_H)
#define CPPREFL_GENERATED_CODE_MACROS_H

#define CPPREFL_INTERNAL_COMBINE2(A,B,C) A##B##C
#define CPPREFL_INTERNAL_COMBINE(A,B,C) CPPREFL_INTERNAL_COMBINE2(A,B,C)

#define CPPREFL_INTERNAL_TO_STR2(X) #X
#define CPPREFL_INTERNAL_TO_STR(X) CPPREFL_INTERNAL_TO_STR2(X)

#define CPPREFL_INTERNAL_GENERATED_REFLECTION_CODE_MARKER __CppReflGeneratedReflectionCodeMarker
#define CPPREFL_INTERNAL_GENERATED_REFLECTION_CODE_MARKER_STR CPPREFL_INTERNAL_TO_STR(CPPREFL_INTERNAL_GENERATED_REFLECTION_CODE_MARKER)

#endif // !defined(CPPREFL_GENERATED_CODE_MACROS_H)

// Always redefine this macro when this file is included.
#undef GENERATED_REFLECTION_CODE

// By default, this is defined as zero. Whenever we invoke the reflection compiler, we'll define this.
#if !defined(CPPREFL_BUILD_REFLECTION)
#define CPPREFL_BUILD_REFLECTION() 0
#endif

#if CPPREFL_BUILD_REFLECTION()

// Define a special marker so the reflection compiler can find which line this macro is used at.
#define GENERATED_REFLECTION_CODE()

#elif defined(CPPREFL_INTERNAL_FILE_ID)

// Calls a special macro defined in the .reflgen.h file. This uses the line number so we can have multiple reflected classes in one file.
#define GENERATED_REFLECTION_CODE() CPPREFL_INTERNAL_COMBINE(__CPPREFLPRIVATE_REFLECTION_CODE_, CPPREFL_INTERNAL_FILE_ID, __LINE__)()

#else

// Define an empty macro so classes can always have it defined, regardless of whether or not the reflection compiler has been run.
#define GENERATED_REFLECTION_CODE()

#endif

namespace cpprefl
{
	class ClassInfo;
	class EnumInfo;
	class TypeInfo;
}