target_sources(CppRefl 
	PUBLIC
	CppReflMarkup.h
	CppReflMemory.h
	CppReflStatics.h

	PRIVATE
	CppReflMemory.cpp
)

add_subdirectory("Private")
add_subdirectory("Reflection")