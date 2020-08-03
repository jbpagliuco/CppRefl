
namespace refl
{
	template <typename T>
	T* Field::GetDataPtr(void* blob)const
	{
		return static_cast<T*>(GetRawDataPtr(blob));
	}


	template <typename ObjectType>
	void Function::Invoke(ObjectType& obj)const
	{
		if (mFunction != nullptr) {
			mFunction((void*)&obj, nullptr, nullptr);
		}
	}

	template <typename ReturnType, typename ObjectType>
	std::optional<ReturnType> Function::Invoke(ObjectType& obj)const
	{
		if (mFunction != nullptr) {
			ReturnType rv;
			mFunction((void*)&obj, &rv, nullptr);
			
			return { rv };
		}
		
		return std::nullopt;
	}

	template <typename ObjectType, typename ParamType>
	void Function::Invoke(ObjectType& obj, ParamType& param)const
	{
		if (mFunction != nullptr) {
			mFunction((void*)&obj, nullptr, (void*)&param);
		}
	}

	template <typename ReturnType, typename ObjectType, typename ParamType>
	std::optional<ReturnType> Function::Invoke(ObjectType& obj, ParamType& param)const
	{
		if (mFunction != nullptr) {
			ReturnType rv;
			mFunction((void*)&obj, &rv, (void*)&param);

			return { rv };
		}

		return std::nullopt;
	}
}