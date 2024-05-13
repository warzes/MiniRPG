#pragma once

// Fixed-size array container indexed by an enumeration
template <typename Enum, typename Value, std::size_t Count>
struct EnumArray : public std::array<Value, Count>
{
	static_assert(std::is_enum_v<Enum>, "Enum type parameter must be an enumeration");

	// Returns a reference to the element associated to specified \a key
	constexpr Value& operator[](Enum key)
	{
		const auto index = static_cast<std::size_t>(key);
		assert(index < Count && "Index is out of bounds");
		return std::array<Value, Count>::operator[](index);
	}

	// Returns a reference to the element associated to specified \a key
	constexpr const Value& operator[](Enum key) const
	{
		const auto index = static_cast<std::size_t>(key);
		assert(index < Count && "Index is out of bounds");
		return std::array<Value, Count>::operator[](index);
	}
};