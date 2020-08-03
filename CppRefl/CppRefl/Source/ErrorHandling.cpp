#include "ErrorHandling.h"

#include <stdarg.h>
#include <stdio.h>

namespace refl
{
	static ErrorHandler CustomErrorHandler = nullptr;

	void RaiseErrorInternal(const char* file, int line, const char* fmt, ...)
	{
		if (CustomErrorHandler == nullptr) {
			return;
		}

		char buffer[1024];

		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, 1024, fmt, args);
		va_end(args);

		CustomErrorHandler(buffer, file, line);
	}

	void SetErrorHandler(ErrorHandler errorHandler)
	{
		CustomErrorHandler = errorHandler;
	}
}