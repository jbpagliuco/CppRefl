#pragma once

namespace refl
{
#if defined(CPP_REFL_BUILD_REFLECTION)
#define _CPP_REFLECTED_ANNOTATION "cpp_refl"

#define CPP_REFLECTED __attribute__((annotate(_CPP_REFLECTED_ANNOTATION)))
#else
#define CPP_REFLECTED
#endif
}