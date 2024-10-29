target_sources(CppRefl 
	PUBLIC
	CppReflConfig.h
	CppReflHash.h
	CppReflMarkup.h
	CppReflStatics.h

	PRIVATE
	CppReflConfig.cpp
)

add_subdirectory("Private")
add_subdirectory("Reflection")
add_subdirectory("Serialization")