#pragma once

#ifndef CPPREFL_STORE_NAMES
#define CPPREFL_STORE_NAMES() _DEBUG
#endif

#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
#define CPPREFL_CONCEPTS() 1
#else
#define CPPREFL_CONCEPTS() 0
#endif