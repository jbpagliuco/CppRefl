#include "ReflectionFunctionRegistration.h"

namespace refl
{
	std::vector<FunctionRegistration*>& GetFunctionRegistrations()
	{
		// Must be wrapped in a function to make sure it's initialized before use.
		static std::vector<FunctionRegistration*> FunctionRegistrations;

		return FunctionRegistrations;
	}

	FunctionRegistration::FunctionRegistration(const std::string& functionName, FunctionType function, void* functionInvoker) :
		mQualifiedClassName(""),
		mFunctionName(functionName),
		mFunction(function),
		mFunctionInvoker(functionInvoker)
	{
		GetFunctionRegistrations().push_back(this);
	}

	FunctionRegistration::FunctionRegistration(const std::string& qualifiedClassName, const std::string& functionName, FunctionType function, void* functionInvoker) :
		mQualifiedClassName(qualifiedClassName),
		mFunctionName(functionName),
		mFunction(function),
		mFunctionInvoker(functionInvoker)
	{
		GetFunctionRegistrations().push_back(this);
	}
}