#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace cpprefl
{
	template <typename ElementType, typename SizeType = uint8_t>
	class Span
	{
	public:
		// Constructs an empty span.
		constexpr Span() : mSize(0), mElements(nullptr) {}

		// Constructs a span from an array.
		template <SizeType N>
		constexpr Span(const ElementType elements[N]) : mSize(N), mElements(elements) {}

		// Constructs a span from an array.
		template <SizeType N>
		constexpr Span(const std::array<ElementType, N>& elements) : mSize(N), mElements(elements.data()) {}

		// Constructs a span from a vector.
		constexpr Span(const std::vector<ElementType>& elements) : mSize((SizeType)elements.size()), mElements(elements.data()) {}

		// Returns the size of this span.
		constexpr SizeType size()const { return mSize; }

		constexpr auto begin()  { return mElements; }
		constexpr auto begin()const  { return mElements; }
		constexpr auto cbegin()const  { return mElements; }
		constexpr auto end()  { return mElements + mSize; }
		constexpr auto end()const  { return mElements + mSize; }
		constexpr auto cend()const  { return mElements + mSize; }

		// Returns the element at a given index.
		constexpr const ElementType& operator[](SizeType index)const { return mElements[index]; }

		// Returns if this span contains an element.
		constexpr bool Contains(const ElementType& element)const
		{
			for (const auto& it : *this)
			{
				if (it == element)
				{
					return true;
				}
			}

			return false;
		}

		// Returns if this span contains an element.
		template <typename EqualityFunction>
		constexpr bool Contains(const ElementType& element, EqualityFunction equalityFunction)const
		{
			for (const auto& it : *this)
			{
				if (equalityFunction(it, element))
				{
					return true;
				}
			}

			return false;
		}

		// Returns if this span contains an element.
		template <typename T, typename EqualityFunction>
		constexpr bool Contains(const T& element, EqualityFunction equalityFunction)const
		{
			for (const auto& it : *this)
			{
				if (equalityFunction(it, element))
				{
					return true;
				}
			}

			return false;
		}

		// Returns if this span contains an element.
		template <typename T, typename EqualityFunction>
		constexpr const ElementType* Find(const T& element, EqualityFunction equalityFunction)const
		{
			for (const auto& it : *this)
			{
				if (equalityFunction(it, element))
				{
					return &it;
				}
			}

			return nullptr;
		}

	private:
		SizeType mSize;
		const ElementType* mElements;
	};
}
