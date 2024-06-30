# CppRefl
CppRefl is a C++ reflection compiler and code generator. It uses Clang to parse reflection markup in your source code, and then generates code that can be used for runtime reflection purposes. CppRefl allows users to write completely custom code generators for their specific libraries without every needing to touch the original CppRefl source code.

### Why Use Reflection?
Reflection is an incredibly powerful tool that gives you access to the layout of your program at runtime. This enables you to do things like:
- Look up and invoke a function by name (e.g. for console commands, cross-programming language communication).
- Write code that can serialize the fields in a class in a generic manner.

### Why Use Code Generation?
Oftentimes you might create commonly useful functions for multiple different classes, or maybe add special values to an enum to mark the max/min values. With C++, most of the time you can achieve these things using preprocessor macros or maybe templates. While this can work for most cases, they cannot handle everything without getting unwieldly (ever seen how to overload macros?). And for anything even remotely complex, you often end up with code that is incredibly hard to read and debug. Real code generation cuts down on the weirdness while giving you maximum power to get you what you're looking for.

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
  SideQuest
};

// Example of how to use reflected enums. (NB: Output is only a rough example of what you'd see)

// Ways to get access to the reflected class:
> const cpprefl::EnumInfo* enumInfo = cpprefl::Registry::GetSystemRegistry().TryGetEnum("ReflectedEnum")
> const cpprefl::EnumInfo& enumInfo = cpprefl::GetReflectedEnum<MissionType>()

// Get the values in an enum.
> cpprefl::GetReflectedEnum<MissionType>().mValues
{ mName = "MainStory", mValue = 0 }
{ mName = "SideQuest", mValue = 1 }
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
> const cpprefl::ClassInfo* classInfo = cpprefl::Registry::GetSystemRegistry().TryGetClass("ShaderFloatParameter")
> const cpprefl::ClassInfo& classInfo = cpprefl::GetReflectedClass<ShaderFloatParameter>()
> const cpprefl::ClassInfo& classInfo = ShaderFloatParameter::StaticReflectedClass()
> ShaderFloatParameter f; const cpprefl::ClassInfo& classInfo = f.GetReflectedClass()

// Get the fields of a class.
> cpprefl::GetReflectedClass<ShaderFloatParameter>().mFields
{ mName = "ParameterName", mType = "cpprefl::TypeInfo{std::string}" }
{ mName = "ParameterValue", mType = "cpprefl::TypeInfo{float}" }
```
