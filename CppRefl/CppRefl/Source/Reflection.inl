
namespace refl
{
	///////////////////////////////////////////////////////////
	// Field
	///////////////////////////////////////////////////////////

	template <typename T>
	T* Field::GetDataPtr(void* obj)const
	{
		return static_cast<T*>(GetRawDataPtr(obj));
	}


	///////////////////////////////////////////////////////////
	// Function
	///////////////////////////////////////////////////////////

	// Custom implementation of std::forward that also cast the argument to a nullptr.
	template <class _Ty>
	constexpr void* forward_and_cast_to_void_ptr(std::remove_reference_t<_Ty>&& _Arg)
	{
		static_assert(!std::is_lvalue_reference_v<_Ty>, "bad forward call");
		return static_cast<void*>(&_Arg);
	}

	// Custom implementation of std::forward that also cast the argument to a nullptr.
	template <class _Ty>
	constexpr void* forward_and_cast_to_void_ptr(std::remove_reference_t<_Ty>& _Arg)
	{
		return static_cast<void*>(&_Arg);
	}

	template <typename... ParamTypes>
	void Function::Invoke(void* obj, ParamTypes&&... params)const
	{
		InvokeInternal(obj, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	template <typename ReturnType, typename... ParamTypes>
	std::optional<ReturnType> Function::Invoke(void* obj, ParamTypes&&... params)const
	{
		return InvokeInternal<ReturnType>(obj, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	template <typename ObjectType, typename... ParamTypes>
	void Function::Invoke(ObjectType& obj, ParamTypes&&... params)const
	{
		InvokeInternal((void*)&obj, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	template <typename ReturnType, typename ObjectType, typename... ParamTypes>
	std::optional<ReturnType> Function::Invoke(ObjectType& obj, ParamTypes&&... params)const
	{
		return InvokeInternal<ReturnType>((void*)&obj, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	// The real invoker.
	template <typename ReturnType>
	std::optional<ReturnType> Function::InvokeInternal(void* obj_or_null, void* param1_or_null)const
	{
		if (mFunction != nullptr) {
			ReturnType rv;
			mFunction(obj_or_null, &rv, param1_or_null);

			if (mReturnType != Type::VOID) {
				return { rv };
			}
			else {
				REFL_RAISE_ERROR_INTERNAL("Tried to read return value from a function that doesn't return a value.");
			}
		}

		return std::nullopt;
	}
}