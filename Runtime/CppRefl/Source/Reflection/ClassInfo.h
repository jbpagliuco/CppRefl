#pragma once

#include "FieldInfo.h"
#include "ObjectInfo.h"

namespace cpprefl
{
	class FieldInfo;
	class TypeInfo;

	using FieldView = Span<FieldInfo>;

	using ClassConstructor = void(*)(void*);
	using ClassDestructor = void(*)(void*);

	// Information about a reflected class.
	class ClassInfo : public ObjectInfo
	{
	public:
		ClassInfo(
			const TypeInfo& type,
			const ClassInfo* baseClass,
			ClassConstructor ctor,
			ClassDestructor dtor,
			const FieldView& fields, 
			const MetadataTagView& tags, 
			const MetadataAttributeView& attributes) : ObjectInfo(tags, attributes), mType(type), mBaseClass(baseClass), mConstructor(ctor), mDestructor(dtor), mFields(fields)
		{
		}

		// The type of this class.
		const TypeInfo& mType;

		// Base class.
		const ClassInfo* mBaseClass;

		// Class constructor.
		ClassConstructor mConstructor;
		ClassDestructor mDestructor;

		// All fields in this class.
		FieldView mFields;

	public:
		void Construct(void* obj)const;
		void Destruct(void* obj)const;

		// Returns if this class is a child of the given class.
		bool IsA(const ClassInfo& baseClass)const;

		// Returns if this class is a child of the given class.
		template <typename T>
		bool IsA()const
		{
			return IsA(GetReflectedClass<T>());
		}

		const FieldInfo* GetField(const Name& fieldName)const;

		// Returns the value of a field in memory, but does not do any validation on if the given template type matches the actual field type.
		template <typename T>
		T* GetFieldValueUnsafe(void* classObject, const Name& fieldName)const;

		// Returns the value of a field in memory.
		// If the templated type does not match the field type, nullptr will be returned.
		template <typename T>
		T* GetFieldValueSafe(void* classObject, const Name& fieldName)const;
	};

	template <typename T>
	T* ClassInfo::GetFieldValueUnsafe(void* classObject, const Name& fieldName) const
	{
		const FieldInfo* fieldInfo = GetField(fieldName);
		return fieldInfo != nullptr ? fieldInfo->GetMemoryInClass<T>(classObject) : nullptr;
	}

	template <typename T>
	T* ClassInfo::GetFieldValueSafe(void* classObject, const Name& fieldName) const
	{
		const FieldInfo* fieldInfo = GetField(fieldName);
		if (fieldInfo != nullptr && IsSameType<T>(fieldInfo->GetType().mKind))
		{
			return fieldInfo != nullptr ? fieldInfo->GetMemoryInClass<T>(classObject) : nullptr;
		}

		return nullptr;
	}
}
