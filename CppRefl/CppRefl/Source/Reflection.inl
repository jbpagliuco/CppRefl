
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


	// Invoke a reflected global function with no return value.
	template <typename... ParamTypes>
	void Function::InvokeGlobal(ParamTypes&&... params)const
	{
		InvokeInternal(nullptr, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	// Invoke a reflected global function that returns a value.
	template <typename ReturnType, typename... ParamTypes>
	std::optional<ReturnType> Function::InvokeGlobal(ParamTypes&&... params)const
	{
		return InvokeInternal<ReturnType>(nullptr, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	// Invoke a reflected member function with no return value.
	template <typename... ParamTypes>
	void Function::InvokeMember(void* obj, ParamTypes&&... params)const
	{
		InvokeInternal(obj, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	// Invoke a reflected member function that returns a value.
	template <typename ReturnType, typename... ParamTypes>
	std::optional<ReturnType> Function::InvokeMember(void* obj, ParamTypes&&... params)const
	{
		return InvokeInternal<ReturnType>(obj, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	// Invoke a reflected member function with no return value.
	template <typename ObjectType, typename... ParamTypes>
	void Function::InvokeMember(ObjectType& obj, ParamTypes&&... params)const
	{
		InvokeInternal((void*)&obj, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	// Invoke a reflected member function that returns a value.
	template <typename ReturnType, typename ObjectType, typename... ParamTypes>
	std::optional<ReturnType> Function::InvokeMember(ObjectType& obj, ParamTypes&&... params)const
	{
		return InvokeInternal<ReturnType>((void*)&obj, forward_and_cast_to_void_ptr<ParamTypes>(params)...);
	}

	// The real invoker.
	template <typename ReturnType>
	std::optional<ReturnType> Function::InvokeInternal(void* obj_or_null, void* param1_or_null)const
	{
		if (ValidateInvocation(obj_or_null, param1_or_null)) {
			ReturnType rv;
			mFunction(mFunctionInvoker, obj_or_null, &rv, param1_or_null);

			if (mReturnType != Type::VOID) {
				return { rv };
			}
			else {
				REFL_INTERNAL_RAISE_ERROR("Tried to read return value from a function that doesn't return a value.");
			}
		}

		return std::nullopt;
	}
}