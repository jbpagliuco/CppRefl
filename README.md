# CppRefl
CppRefl is a C++ reflection compiler and code generator. It uses Clang to parse reflection markup in your source code, and then generates code that can be used for runtime reflection purposes. CppRefl also allows users to write completely custom code generators for their specific libraries without every needing to touch the original CppRefl source code.

### Why Use Reflection?
Reflection is an incredibly powerful tool that gives you access to the layout of your program at runtime. This enables you to do things like:
- Look up and invoke a function by name (great for communication across languages).
- Write code the can serialize the data in a class, without needing to explicitly know the layout of that class.

### Why Use Code Generation?
Oftentimes in C++ code, you can achieve a lite version of code generation using preprocessor macros. While this can work for most cases, macros cannot handle everything. And for anything even remotely complex, you often end up with code that is incredibly hard to read and debug.

---

# Supported Features
- Enums
- Classes/Structs
  - Fields
  - Member functions
- Static/Global functions
- Namespaces
- Templated Classes (Mostly)
- Aliases/Typedefs (Somewhat)

# Example Usage
### Reflected Enum
```cpp
// Includes the generated code for this file (which is called "MissionType.h"). 
#include "MissionType.reflgen.h"

// Reflected enum representing a mission type.
enum class REFLECTED MissionType
{
  MainStory = 0,
  OpenWorld
};

// Example of how to use reflected enums. (NB: Output is only a rough example of what you'd see)

// Ways to get access to the reflected class:
> cpprefl::Registry::GetSystemRegistry().GetEnum("ReflectedEnum")
> cpprefl::GetReflectedEnum<MissionType>()

// Get the values in an enum.
> cpprefl::GetReflectedEnum<MissionType>().mValues
{ mName = "MainStory", mValue = 0 }
{ mName = "OpenWorld", mValue = 1 }
```

### Reflected Class
```cpp
// Includes the generated code for this file (which is called "ShaderFloatParameter.h"). 
#include "ShaderFloatParameter.reflgen.h"

// Reflected class representing a shader value.
class REFLECTED ShaderFloatParameter
{
  GENERATED_REFLECTION_CODE()

public:
  std::string ParameterName REFLECTED;
  float ParameterValue REFLECTED;
};

// Example of how to use reflected classes (NB: Output is only a rough example of what you'd see).

// Ways to get access to the reflected class:
> cpprefl::Registry::GetSystemRegistry().GetClass("ShaderFloatParameter")
> cpprefl::GetReflectedClass<ShaderFloatParameter>()
> ShaderFloatParameter::StaticReflectedClass()
> ShaderFloatParameter f; f.GetReflectedClass()

// Get the fields of a class.
> cpprefl::GetReflectedClass<ShaderFloatParameter>().mFields
{ mName = "ParameterName", mType = "cpprefl::TypeInfo{std::string}" }
{ mName = "ParameterValue", mType = "cpprefl::TypeInfo{float}" }
```

---

# Project Layout
## C++
- CppRefl: C++ static library that implements the reflection runtime.
- CppRefl.UnitTest.Cpp: Unit tests for C++ code.

## C#/.NET
- CppRefl.CodeGeneration: Library containing functionality for generating code based of a program's reflection information.
- CppRefl.Compiler: Library that generates compiler agnostic reflection information about a program. Currently this uses Clang to parse the source program. (NB: This is a separate library from the CLI to simplify our unit tests).
- CppRefl.Compiler.CLI: Small CLI wrapper over CppRefl.Compiler.
- CppRefl.UnitTest.CSharp: Unit tests for C# code.
