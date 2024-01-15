project("CppRefl.Tests")

# C++17 is the minimum supported language version.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable("CppRefl.Tests" "")

target_link_libraries("CppRefl.Tests" CppRefl)

target_include_directories("CppRefl.Tests" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/GeneratedCode")
target_include_directories("CppRefl.Tests" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Source/ReflectedCode")

set(REFLECTED_FILES
	# AliasCode
	ClassCode
	EnumCode
	# FunctionCode
	MetadataCode
	# SerializerCode
	# TemplateCode
	TypesCode
)
set(REFLECTED_MODULE_FILES
	${CMAKE_CURRENT_LIST_DIR}/GeneratedCode/CppReflTests.reflgen.h
	${CMAKE_CURRENT_LIST_DIR}/GeneratedCode/CppReflTests.reflgen.cpp
)

foreach(FILE IN LISTS REFLECTED_FILES)
	list(APPEND GENERATED_FILES "${CMAKE_CURRENT_LIST_DIR}/GeneratedCode/${FILE}.reflgen.h" "${CMAKE_CURRENT_LIST_DIR}/GeneratedCode/${FILE}.reflgen.cpp")
	list(APPEND REFLECTED_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Source/ReflectedCode/${FILE}.h")
endforeach()

foreach(FILE IN LISTS GENERATED_FILES)
	target_sources(CppRefl.Tests PRIVATE "${FILE}")
endforeach()

foreach(FILE IN LISTS REFLECTED_HEADERS)
	target_sources(CppRefl.Tests PRIVATE "${FILE}")
endforeach()

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/GeneratedCode" PREFIX "Generated Files" FILES ${GENERATED_FILES})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/GeneratedCode" PREFIX "Generated Files" FILES ${REFLECTED_MODULE_FILES})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/Source/ReflectedCode" PREFIX "Reflected Headers" FILES ${REFLECTED_HEADERS})

set(TEST_FILES
	Source/Tests/ClassTests.cpp
	Source/Tests/EnumTests.cpp
	Source/Tests/FunctionTests.cpp
	Source/Tests/MetadataTests.cpp
	Source/Tests/SerializerTests.cpp
	Source/Tests/TypesTests.cpp
)

foreach(FILE IN LISTS TEST_FILES)
	target_sources(CppRefl.Tests PRIVATE "${CMAKE_CURRENT_LIST_DIR}/${FILE}")
endforeach()
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/Source/Tests" PREFIX "Tests" FILES ${TEST_FILES})

target_sources("CppRefl.Tests" PRIVATE
	${REFLECTED_MODULE_FILES}
	Source/Main.cpp
)

set(GTEST_DIR "${CMAKE_CURRENT_LIST_DIR}/packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.7")
target_include_directories("CppRefl.Tests" PRIVATE "${GTEST_DIR}/build/native/include")

set(GTEST_LIB_DIR "${GTEST_DIR}/lib/native/v140/windesktop/msvcstl/static/rt-dyn/x64")
target_link_libraries("CppRefl.Tests" debug "${GTEST_LIB_DIR}/Debug/gtestd.lib" optimized "${GTEST_LIB_DIR}/Release/gtest.lib")