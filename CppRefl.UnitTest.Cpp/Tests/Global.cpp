#include "gtest/gtest.h"

#include "GeneratedCode/CppReflUnitTest.reflregistry.h"

int main(int argc, char** argv)
{
    RegisterCppReflUnitTestReflection();

    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}