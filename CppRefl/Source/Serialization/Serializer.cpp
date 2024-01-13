#include "Serializer.h"

#include "../CppReflMemory.h"

namespace cpprefl::serialization
{
	bool Deserializer::Deserialize(IObjectDeserializer& deserializer, void* classMemory, const ClassInfo& classInfo)
	{
		const auto rootDeserializationObject = deserializer.GetRootObject();
		if (rootDeserializationObject == nullptr)
		{
			return false;
		}

		return DeserializeClass(deserializer, rootDeserializationObject, classMemory, classInfo);
	}

	void Deserializer::RegisterExtension(IObjectDeserializerExtension& extension)
	{
		mExtensions.push_back(&extension);
	}

	bool Deserializer::DeserializeClass(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, bool construct)
	{
		if (construct)
		{
			classInfo.Construct(classMemory);
		}

		// Deserialize base class
		if (classInfo.mBaseClass != nullptr)
		{
			DeserializeClass(deserializer, deserializationObject, classMemory, *classInfo.mBaseClass, false);
		}

		// Deserialize class
		for (const auto& fieldInfo : classInfo.mFields)
		{
			if (CanDeserialize(classMemory, classInfo, fieldInfo))
			{
				const auto subDeserializationObject = deserializer.GetSubobject(deserializationObject, fieldInfo.mName);
				if (subDeserializationObject != nullptr)
				{
					const auto fieldType = GetFieldType(fieldInfo.mTypeInstance);
					DeserializeField(deserializer, subDeserializationObject, classMemory, classInfo, fieldInfo.GetMemoryInClass(classMemory), fieldInfo, fieldType);
				}
			}
		}

		return true;
	}

	bool Deserializer::DeserializeArray(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, void* fieldMemory, const FieldInfo& fieldInfo)
	{
		const auto dynamicArrayFunctions = Registry::GetSystemRegistry().GetDynamicArrayFunctions(fieldInfo.GetType().mName);
		const auto elementTypeInstance = dynamicArrayFunctions != nullptr ? dynamicArrayFunctions->mElementType : fieldInfo.mTypeInstance;
		const auto fieldDeserializationType = GetFieldType(elementTypeInstance.mType);

		ArraySizeType numElements = deserializer.GetArrayLength(deserializationObject);
		void* arrayDataMemory = fieldMemory;
		if (dynamicArrayFunctions != nullptr)
		{
			dynamicArrayFunctions->mSetSize(fieldMemory, numElements);
			arrayDataMemory = dynamicArrayFunctions->mGetData(fieldMemory);
		}
		else
		{
			numElements = std::min(numElements, fieldInfo.mTypeInstance.mArraySize);
		}

		for (ArraySizeType i = 0; i < numElements; ++i)
		{
			void* elementMemory = (std::byte*)arrayDataMemory + (i * elementTypeInstance.mType.mSize);
			void* deserializationSubobject = deserializer.GetSubobject(deserializationObject, (int)i);
			DeserializeField(deserializer, deserializationSubobject, classMemory, classInfo, elementMemory, fieldInfo, fieldDeserializationType, &elementTypeInstance);
		}

		return true;
	}

	bool Deserializer::DeserializeField(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, void* fieldMemory, const FieldInfo& fieldInfo, FieldDeserializationType fieldDeserializationType, const TypeInstanceInfo* typeInstance)
	{
		switch (fieldDeserializationType)
		{
		case FieldDeserializationType::Object:
		{
			// Deserialize a nested class.
			const auto dynamicClassType = deserializer.GetDynamicClassType(deserializationObject);
			if (dynamicClassType.has_value())
			{
				const auto dynamicClassInfo = Registry::GetSystemRegistry().TryGetClass(dynamicClassType->c_str());
				if (dynamicClassInfo != nullptr)
				{
					char* buffer = AllocateMemoryIntoField(fieldMemory, dynamicClassInfo->mType.mSize);
					DeserializeClass(deserializer, deserializationObject, buffer, *dynamicClassInfo);
				}
			}
			else
			{
				const TypeInfo& typeInfo = typeInstance != nullptr ? typeInstance->mType : fieldInfo.GetType();
				DeserializeClass(deserializer, deserializationObject, fieldMemory, typeInfo.GetClassInfo(), false);
			}
			break;
		}

		case FieldDeserializationType::Array:
			// Deserialize a nested array.
			DeserializeArray(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);
			break;

		case FieldDeserializationType::Value:
			// Deserialize a raw value.
			DeserializeValue(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo, typeInstance);
			break;
		}

		return true;
	}

	bool Deserializer::DeserializeValue(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, void* fieldMemory, const FieldInfo& fieldInfo, const TypeInstanceInfo* typeInstance)
	{
		const TypeInstanceInfo& fieldTypeInstance = typeInstance != nullptr ? *typeInstance : fieldInfo.mTypeInstance;
		const TypeInfo& fieldType = fieldTypeInstance.mType;

		if (fieldTypeInstance.IsString())
		{
			String stringValue;
			if (deserializer.DeserializeValue(deserializationObject, stringValue))
			{
				if (fieldTypeInstance.IsDynamicString())
				{
					const auto stringSize = strlen(stringValue) + 1;
					char* buffer = AllocateMemoryIntoField(fieldMemory, stringSize);
					strcpy_s(buffer, stringSize, stringValue);
				}
				else
				{
					strcpy_s((char*)fieldMemory, fieldTypeInstance.mArraySize, stringValue);
				}

				return true;
			}

			return false;
		}

		switch (fieldType.mKind)
		{
		case TypeKind::Bool:
			return DeserializeAndModifyValue<bool, bool>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Uint8:
			return DeserializeAndModifyValue<uint64_t, uint8_t>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Int8:
			return DeserializeAndModifyValue<int64_t, int8_t>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Uint16:
			return DeserializeAndModifyValue<uint64_t, uint16_t>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Int16:
			return DeserializeAndModifyValue<int64_t, int16_t>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Uint32:
			return DeserializeAndModifyValue<uint64_t, uint32_t>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Int32:
			return DeserializeAndModifyValue<int64_t, int32_t>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Uint64:
			return DeserializeAndModifyValue<uint64_t, uint64_t>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Int64:
			return DeserializeAndModifyValue<int64_t, int64_t>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Float:
			return DeserializeAndModifyValue<double, float>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Double:
			return DeserializeAndModifyValue<double, double>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::LongDouble:
			return DeserializeAndModifyValue<double, long double>(deserializer, deserializationObject, classMemory, classInfo, fieldMemory, fieldInfo);

		case TypeKind::Enum:
			return DeserializeEnumValue(deserializer, deserializationObject, fieldMemory, fieldInfo);
		}

		return false;
	}

	bool Deserializer::DeserializeEnumValue(IObjectDeserializer& deserializer, void* deserializationObject, void* fieldMemory, const FieldInfo& fieldInfo)
	{
		String stringValue;
		if (deserializer.DeserializeValue(deserializationObject, stringValue))
		{
			const auto& enumInfo = fieldInfo.mTypeInstance.mType.GetEnumInfo();
			const auto& enumValue = enumInfo.GetValue(stringValue);
			if (enumValue != nullptr)
			{
				switch (fieldInfo.GetType().mSize)
				{
				case 1:
					*(int8_t*)fieldMemory = (int8_t)enumValue->mValue;
					return true;

				case 2:
					*(int16_t*)fieldMemory = (int16_t)enumValue->mValue;
					return true;

				case 4:
					*(int32_t*)fieldMemory = (int32_t)enumValue->mValue;
					return true;

				case 8:
					*(int64_t*)fieldMemory = (int64_t)enumValue->mValue;
					return true;

				default:
					break;
				}
			}
		}

		return false;
	}

	FieldDeserializationType Deserializer::GetFieldType(const TypeInfo& typeInfo)
	{
		if (typeInfo.mKind == TypeKind::Class)
		{
			return FieldDeserializationType::Object;
		}

		return FieldDeserializationType::Value;
	}

	FieldDeserializationType Deserializer::GetFieldType(const TypeInstanceInfo& typeInfo)
	{
		const bool isArray = typeInfo.mIsArray || Registry::GetSystemRegistry().GetDynamicArrayFunctions(typeInfo.mType.mName);
		const bool isString = typeInfo.IsFixedSizeString();
		if (isArray && !isString)
		{
			return FieldDeserializationType::Array;
		}

		return GetFieldType(typeInfo.mType);
	}

	bool Deserializer::CanDeserialize(void* classMemory, const ClassInfo& classInfo, const FieldInfo& fieldInfo) const
	{
		for (const auto& extension : mExtensions)
		{
			if (!extension->CanDeserialize(classMemory, classInfo, fieldInfo))
			{
				return false;
			}
		}

		return true;
	}

	char* Deserializer::AllocateMemoryIntoField(void* fieldMemory, size_t numBytes)
	{
		auto& memoryAllocator = GetMemoryAllocator();
		char* buffer = (char*)memoryAllocator.Allocate(numBytes);

		*(char**)fieldMemory = buffer;
		return buffer;
	}

	template <typename ModificationType, typename RealType>
	bool Deserializer::DeserializeAndModifyValue(IObjectDeserializer& deserializer, void* deserializationObject, void* classMemory, const ClassInfo& classInfo, void* fieldMemory, const FieldInfo& fieldInfo)
	{
		auto& value = *(RealType*)fieldMemory;
		if (deserializer.DeserializeValue(deserializationObject, value))
		{
			auto temp = (ModificationType)value;
			for (const auto& extension : mExtensions)
			{
				extension->ModifyValue(classMemory, classInfo, fieldInfo, temp);
			}
			value = (RealType)temp;

			return true;
		}

		return false;
	}
}
