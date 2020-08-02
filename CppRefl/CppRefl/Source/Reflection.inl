
namespace refl
{
	template <typename T>
	T* Field::GetDataPtr(void* blob)const
	{
		return static_cast<T*>(GetRawDataPtr(blob));
	}


	template <typename ReturnType>
	void Function::Invoke(void* self, ReturnType &rv)const
	{
		if (mFunction != nullptr) {
			mFunction(self, &rv);
		}
	}
}