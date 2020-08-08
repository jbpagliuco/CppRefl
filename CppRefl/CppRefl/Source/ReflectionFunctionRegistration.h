#pragma once

#include <string>
#include <vector>

namespace refl
{
	// Function pointer type. This must include the maximum number of
	// allowed parameters (any of which can be null depending on the real reflected function definition).
	typedef void (*FunctionType)(void* functionInvoker, void* obj, void* returnValue, void* param1);

	// Registers a function with the reflection at runtime.
	class FunctionRegistration
	{
	public:
		// Registers a global function.
		FunctionRegistration(const std::string& functionName, FunctionType function, void* functionInvoker);

		// Registers a class member function.
		FunctionRegistration(const std::string& qualifiedClassName, const std::string& functionName, FunctionType function, void* functionInvoker);

	public:
		// Fully qualified class name.
		std::string mQualifiedClassName;

		// Function name.
		std::string mFunctionName;

		// Pointer to the function invoker
		FunctionType mFunction;
		void* mFunctionInvoker;
	};

	// Get a list of all function registrations.
	std::vector<FunctionRegistration*>& GetFunctionRegistrations();
}