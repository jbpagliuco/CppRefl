#include "ReflectionTypes.h"

namespace refl
{
	TypeInfo TypeInfo::INVALID;

	bool TypeInfo::DeepEquals(const TypeInfo& rhs)const
	{
		return mDataType == rhs.mDataType &&
			mSize == rhs.mSize &&
			mArraySize == rhs.mArraySize &&
			mClassType == rhs.mClassType &&
			mEnumType == rhs.mEnumType &&
			mIsConst == rhs.mIsConst &&
			mIsPointer == rhs.mIsPointer;
	}
}