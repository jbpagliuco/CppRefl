#include <cstdarg>

#include "gtest/gtest.h"

#include "CppReflConfig.h"

#include "CppReflTests.reflgen.h"

class TestConfig : public cpprefl::IConfig
{
public:
#if CPPREFL_LOG()
    void Log(cpprefl::LogLevel level, const char* fmt, ...) override
    {
		va_list args;
		va_start(args, fmt);
		{
			char buf[1024];
			vsprintf_s(buf, fmt, args);

			testing::Message msg;
			msg << buf;
			testing::AssertionFailure(msg);
		}
		va_end(args);
    }
#endif
};

int main(int argc, char** argv)
{
	TestConfig config;
	config.Set();

    RegisterCppReflTestsReflection();

    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();

	using namespace cpprefl;
	CPPREFL_INTERNAL_LOG(cpprefl::LogLevel::Error, "idk");
}