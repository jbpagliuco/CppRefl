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
		Span() : mSize(0), mElements(nullptr) {}

		// Constructs a span from an array.
		template <SizeType N>
		Span(const ElementType elements[N]) : mSize(N), mElements(elements.data()) {}

		// Constructs a span from an array.
		template <SizeType N>
		Span(const std::array<ElementType, N>& elements) : mSize(N), mElements(elements.data()) {}

		// Constructs a span from a vector.
		Span(const std::vector<ElementType>& elements) : mSize((SizeType)elements.size()), mElements(elements.data()) {}

		// Returns the size of this span.
		SizeType size()const { return mSize; }

		auto begin()  { return mElements; }
		auto begin()const  { return mElements; }
		auto cbegin()const  { return mElements; }
		auto end()  { return mElements + mSize; }
		auto end()const  { return mElements + mSize; }
		auto cend()const  { return mElements + mSize; }

		// Returns the element at a given index.
		const ElementType& operator[](SizeType index)const { return mElements[index]; }

	private:
		SizeType mSize;
		const ElementType* mElements;
	};
}
