#include "ClassInfo.h"

#include "FieldInfo.h"

namespace cpprefl
{
	void ClassInfo::Construct(void* obj)const
	{
		mConstructor(obj);
	}

	void ClassInfo::Destruct(void* obj)const
	{
		mDestructor(obj);
	}

	bool ClassInfo::IsA(const ClassInfo& baseClass) const
	{
		const ClassInfo* cls = this;
		while (cls != nullptr)
		{
			if (cls == &baseClass)
			{
				return true;
			}

			cls = cls->mBaseClass;
		}

		return false;
	}

	const FieldInfo* ClassInfo::GetField(const Name& fieldName) const
	{
		for (const auto& fieldInfo : mFields)
		{
			if (fieldInfo.mName == fieldName)
			{
				return &fieldInfo;
			}
		}

		return nullptr;
	}
}
