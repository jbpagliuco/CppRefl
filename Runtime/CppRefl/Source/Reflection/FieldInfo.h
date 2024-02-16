#pragma once

#include "ObjectInfo.h"
#include "TypeInstanceInfo.h"

namespace cpprefl
{
	class TypeInfo;

	// Information about a reflected field on a class.
	class FieldInfo : public ObjectInfo
	{
	public:
		FieldInfo(TypeInstanceInfo type, size_t offset, const char* name, const MetadataTagView& tags, const MetadataAttributeView& attributes) :
			ObjectInfo(tags, attributes),
			mTypeInstance(std::move(type)),
			mOffset(offset),
			mName(name)
		{
		}

		// The type of this field.
		TypeInstanceInfo mTypeInstance;

		// Offset set of this field into the containing class.
		size_t mOffset;

		// Name of this field.
		const char* mName;

	public:
		const TypeInfo& GetType()const { return mTypeInstance.mType; }

		// Returns a raw pointer to this field in a class blob.
		void* GetMemoryInClass(void* classObject)const;

		template <typename T>
		T* GetMemoryInClass(void* classObject)const { return (T*)GetMemoryInClass(classObject); }

		// Returns a raw pointer to the class blob that owns this field.
		void* GetClassObject(void* fieldObject)const;
	};
}
