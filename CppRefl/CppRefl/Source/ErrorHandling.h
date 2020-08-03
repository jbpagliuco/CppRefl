#pragma once

namespace refl
{
	// Raise an error. Used by internal code.
	#define REFL_RAISE_ERROR_INTERNAL(fmt, ...) RaiseErrorInternal(__FILE__, __LINE__, fmt, __VA_ARGS__)
	void RaiseErrorInternal(const char* file, int line, const char* fmt, ...);

	// Sets a custom error handler.
	typedef void(*ErrorHandler)(const char* message, const char* file, int line);
	void SetErrorHandler(ErrorHandler errorHandler);
}