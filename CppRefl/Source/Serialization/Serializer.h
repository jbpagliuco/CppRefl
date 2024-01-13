#pragma once

#include <optional>
#include <string>

#include "../Reflection/ClassInfo.h"
#include "../Reflection/EnumInfo.h"
#include "../Reflection/FieldInfo.h"
#include "../Reflection/Registry.h"
#include "../Reflection/TypeInfo.h"

namespace cpprefl
{
	namespace serialization
	{
		using String = const char*;

		// Class that allows modifying deserialized values before writing them into memory.
		class IObjectDeserializerExtension
		{
		public:
			virtual ~IObjectDeserializerExtension() = default;

			// Determines if a field can be deserialized.
			virtual bool CanDeserialize(void* classObject, const ClassInfo& classInfo, const FieldInfo& fieldInfo) { return true; }

			// Modifies an boolean value.
			virtual void ModifyValue(void* classObject, const ClassInfo& classInfo, const FieldInfo& fieldInfo, bool& value) { }
			// Modifies an integer value.
			virtual void ModifyValue(void* classObject, const ClassInfo& classInfo, const FieldInfo& fieldInfo, int64_t& value) { }
			// Modifies an integer value.
			virtual void ModifyValue(void* classObject, const ClassInfo& classInfo, const FieldInfo& fieldInfo, uint64_t& value) { }
			// Modifies a double value.
			virtual void ModifyValue(void* classObject, const ClassInfo& classInfo, const FieldInfo& fieldInfo, double& value) { }
		};

		enum class FieldDeserializationType
		{
			Object,
			Array,
			Value
		};

		class IObjectDeserializer
		{
		public:
			virtual ~IObjectDeserializer() = default;

			virtual void* GetRootObject() = 0;
			virtual void* GetSubobject(void* object, const char* subobjectName) = 0;
			virtual void* GetSubobject(void* object, int index) = 0;
			virtual ArraySizeType GetArrayLength(void* object) = 0;

			virtual bool DeserializeValue(void* object, bool& outValue) = 0;
			virtual bool DeserializeValue(void* object, uint8_t& outValue) = 0;
			virtual bool DeserializeValue(void* object, int8_t& outValue) = 0;
			virtual bool DeserializeValue(void* object, uint16_t& outValue) = 0;
			virtual bool DeserializeValue(void* object, int16_t& outValue) = 0;
			virtual bool DeserializeValue(void* object, uint32_t& outValue) = 0;
			virtual bool DeserializeValue(void* object, int32_t& outValue) = 0;
			virtual bool DeserializeValue(void* object, uint64_t& outValue) = 0;
			virtual bool DeserializeValue(void* object, int64_t& outValue) = 0;

			virtual bool DeserializeValue(void* object, float& outValue) = 0;
			virtual bool DeserializeValue(void* object, double& outValue) = 0;
			virtual bool DeserializeValue(void* object, long double& outValue) = 0;

			virtual bool DeserializeValue(void* object, String& outValue) = 0;

			virtual std::optional<std::string> GetDynamicClassType(void* object) = 0;
		};

		class Deserializer
		{
		public:
			// Deserialize a class into memory.
			bool Deserialize(IObjectDeserializer& deserializer, void* classMemory, const ClassInfo& classInfo);

			// Deserialize a class into memory.
			template <typename ClassType>
			std::optional<ClassType> Deserialize(IObjectDeserializer& deserializer);

			void RegisterExtension(IObjectDeserializerExtension& extension);

		private:
			bool DeserializeClass(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, bool construct = true);
			bool DeserializeArray(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, void* fieldMemory, const FieldInfo& fieldInfo);
			bool DeserializeField(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, void* fieldMemory, const FieldInfo& fieldInfo, FieldDeserializationType fieldDeserializationType, const TypeInstanceInfo* typeInstance = nullptr);

			bool DeserializeValue(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, void* fieldMemory, const FieldInfo& fieldInfo, const TypeInstanceInfo* typeInstance = nullptr);
			bool DeserializeEnumValue(IObjectDeserializer& deserializer, void* deserializationObject, void* fieldMemory, const FieldInfo& fieldInfo);

			FieldDeserializationType GetFieldType(const TypeInfo& typeInfo);
			FieldDeserializationType GetFieldType(const TypeInstanceInfo& typeInfo);

			bool CanDeserialize(void* classMemory, const ClassInfo& classInfo, const FieldInfo& fieldInfo)const;

			template <typename ModificationType, typename RealType>
			bool DeserializeAndModifyValue(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, void* fieldMemory, const FieldInfo& fieldInfo);

			char* AllocateMemoryIntoField(void* fieldMemory, size_t numBytes);

		private:
			std::vector<IObjectDeserializerExtension*> mExtensions;
		};
	}
}




namespace cpprefl::serialization
{
	template <typename ClassType>
	std::optional<ClassType> Deserializer::Deserialize(IObjectDeserializer& deserializer)
	{
		ClassType obj;
		if (Deserialize(deserializer, &obj, GetReflectedClass<ClassType>()))
		{
			return obj;
		}

		return {};
	}
}
