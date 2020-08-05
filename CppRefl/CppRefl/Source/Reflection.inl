
namespace refl
{
	template <typename T>
	T* Field::GetDataPtr(void* obj)const
	{
		return static_cast<T*>(GetRawDataPtr(obj));
	}



	template <typename ReturnType>
	std::optional<ReturnType> Function::Invoke(void* obj)const
	{
		if (mFunction != nullptr) {
			ReturnType rv;
			mFunction(obj, &rv, nullptr);

			if (mReturnType != Type::VOID) {
				return { rv };
			}
			else {
				REFL_RAISE_ERROR_INTERNAL("Tried to read return value from a function that doesn't return a value.");
			}
		}

		return std::nullopt;
	}

	template <typename ParamType>
	void Function::Invoke(void* obj, ParamType& param)const
	{
		if (mFunction != nullptr) {
			mFunction(obj, nullptr, (void*)&param);
		}
	}

	template <typename ReturnType, typename ParamType>
	std::optional<ReturnType> Function::Invoke(void* obj, ParamType& param)const
	{
		if (mFunction != nullptr) {
			ReturnType rv;
			mFunction(obj, &rv, (void*)&param);

			if (mReturnType != Type::VOID) {
				return { rv };
			}
			else {
				REFL_RAISE_ERROR_INTERNAL("Tried to read return value from a function that doesn't return a value.");
			}
		}

		return std::nullopt;
	}

	template <typename ObjectType>
	void Function::Invoke(ObjectType& obj)const
	{
		Invoke((void*)&obj);
	}

	template <typename ReturnType, typename ObjectType>
	std::optional<ReturnType> Function::Invoke(ObjectType& obj)const
	{
		return Invoke<ReturnType>((void*)&obj);
	}

	template <typename ObjectType, typename ParamType>
	void Function::Invoke(ObjectType& obj, ParamType& param)const
	{
		Invoke((void*)&obj, param);
	}

	template <typename ReturnType, typename ObjectType, typename ParamType>
	std::optional<ReturnType> Function::Invoke(ObjectType& obj, ParamType& param)const
	{
		return Invoke<ReturnType>((void*)&obj, param);
	}
}