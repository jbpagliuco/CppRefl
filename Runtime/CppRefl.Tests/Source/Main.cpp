#include "gtest/gtest.h"

#include "CppReflTests.reflgen.h"

int main(int argc, char** argv)
{
    RegisterCppReflTestsReflection();

    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}