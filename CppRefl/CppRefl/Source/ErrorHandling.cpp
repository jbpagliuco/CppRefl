#include "ErrorHandling.h"

#include <stdarg.h>
#include <stdio.h>

namespace refl
{
	static ErrorHandler CustomErrorHandler = nullptr;

	void RaiseErrorInternal(ErrorType errorType, const char* file, int line, const char* fmt, ...)
	{
		if (CustomErrorHandler == nullptr) {
			__debugbreak();
			return;
		}

		char buffer[1024];

		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, 1024, fmt, args);
		va_end(args);

		CustomErrorHandler(errorType, buffer, file, line);
	}

	void SetErrorHandler(ErrorHandler errorHandler)
	{
		CustomErrorHandler = errorHandler;
	}
}