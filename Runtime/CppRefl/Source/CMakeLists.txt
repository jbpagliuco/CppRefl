target_sources(CppRefl 
	PUBLIC
	CppReflConfig.h
	CppReflHash.h
	CppReflMarkup.h
	CppReflMemory.h
	CppReflStatics.h

	PRIVATE
	CppReflConfig.cpp
	CppReflMemory.cpp
)

add_subdirectory("Private")
add_subdirectory("Reflection")
add_subdirectory("Serialization")