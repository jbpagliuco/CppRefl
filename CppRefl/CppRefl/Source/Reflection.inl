
namespace refl
{
	template <typename T>
	T* Field::GetDataPtr(void* blob)const
	{
		return static_cast<T*>(GetRawDataPtr(blob));
	}


	template <typename ReturnType>
	void Function::Invoke(void* self, ReturnType *rv)const
	{
		if (mFunction != nullptr) {
			mFunction(self, rv, nullptr);
		}
	}

	template <typename ReturnType, typename ParamType>
	void Function::Invoke(void* self, ReturnType *rv, ParamType& param)const
	{
		if (mFunction != nullptr) {
			mFunction(self, rv, (void*)&param);
		}
	}

	template <typename ParamType>
	void Function::Invoke(void* self, std::nullptr_t rv, ParamType& param)const
	{
		if (mFunction != nullptr) {
			mFunction(self, nullptr, (void*)&param);
		}
	}
}