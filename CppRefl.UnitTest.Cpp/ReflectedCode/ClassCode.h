#pragma once

#include "CppReflMarkup.h"

#include "GeneratedCode/ClassCode.reflgen.h"

class REFLECTED ReflectedClass
{
	GENERATED_REFLECTION_CODE()

public:
	~ReflectedClass() { mPublicInt = 666; }

	int mPublicInt REFLECTED = 1234;
	const int mConstInt REFLECTED = 1234;
	int mNotReflectedInt = 10;

protected:
	[[maybe_unused]] float mProtectedFloat REFLECTED = 3.14f;

private:
	[[maybe_unused]] char mPrivateChar REFLECTED = 'a';


public:
	void FuncVoidNoParam() REFLECTED;
	void FuncVoidOneParam(int param1) REFLECTED;
	double FuncDoubleNoParam() REFLECTED;
	double FuncDoubleTwoParam(int param1, short param2) REFLECTED;
};

class NonReflectedClass
{
};

struct REFLECTED ReflectedStruct
{
	GENERATED_REFLECTION_CODE()
};

namespace TestNamespace
{
	class REFLECTED ReflectedClass
	{
		GENERATED_REFLECTION_CODE()

	private:
		int mField REFLECTED;
	};
}





class REFLECTED BaseClass
{
	GENERATED_REFLECTION_CODE()

protected:
	int mBaseClassField REFLECTED;
};

class IInterface
{};

class REFLECTED ChildClass : public BaseClass, public IInterface
{
	GENERATED_REFLECTION_CODE()

public:
	int mChildClassField REFLECTED;
};

class REFLECTED ChildClass2 : public ChildClass
{
	GENERATED_REFLECTION_CODE()
};