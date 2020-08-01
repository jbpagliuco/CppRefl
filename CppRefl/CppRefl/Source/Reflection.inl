
namespace refl
{
	template <typename T>
	T* Field::GetDataPtr(void* blob)const
	{
		return static_cast<T*>(GetRawDataPtr(blob));
	}
}