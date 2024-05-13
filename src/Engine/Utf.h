#pragma once

// References:
//
// https://www.unicode.org/
// https://www.unicode.org/Public/PROGRAMS/CVTUTF/ConvertUTF.c
// https://www.unicode.org/Public/PROGRAMS/CVTUTF/ConvertUTF.h
// https://people.w3.org/rishida/scripts/uniview/conversion

namespace priv
{
	template <typename InputIt, typename OutputIt>
	OutputIt copy(InputIt first, InputIt last, OutputIt dFirst)
	{
		while (first != last)
			*dFirst++ = static_cast<typename OutputIt::container_type::value_type>(*first++);

		return dFirst;
	}
}

template <unsigned int N>
class Utf;

// Specialization of the Utf template for UTF-8
template <>
class Utf<8>
{
public:
	// Decode a single UTF-8 character
	// Decoding a character means finding its unique 32-bits code (called the codepoint) in the Unicode standard.
	template <typename In>
	static In decode(In begin, In end, std::uint32_t& output, std::uint32_t replacement = 0);

	// Encode a single UTF-8 character
	// Encoding a character means converting a unique 32-bits code (called the codepoint) in the target encoding, UTF-8.
	template <typename Out>
	static Out encode(std::uint32_t input, Out output, std::uint8_t replacement = 0);

	// Advance to the next UTF-8 character
	// This function is necessary for multi-elements encodings, as a single character may use more than 1 storage element.
	template <typename In>
	static In next(In begin, In end);

	// Count the number of characters of a UTF-8 sequence
	// This function is necessary for multi-elements encodings, as a single character may use more than 1 storage element, thus the total size can be different from (begin - end).
	template <typename In>
	static std::size_t count(In begin, In end);

	// Convert an ANSI characters range to UTF-8
	// The current global locale will be used by default, unless you pass a custom one in the \a locale parameter.
	template <typename In, typename Out>
	static Out fromAnsi(In begin, In end, Out output, const std::locale& locale = std::locale());

	// Convert a wide characters range to UTF-8
	template <typename In, typename Out>
	static Out fromWide(In begin, In end, Out output);

	// Convert a latin-1 (ISO-5589-1) characters range to UTF-8
	template <typename In, typename Out>
	static Out fromLatin1(In begin, In end, Out output);

	// Convert an UTF-8 characters range to ANSI characters
	// The current global locale will be used by default, unless you pass a custom one in the \a locale parameter.
	template <typename In, typename Out>
	static Out toAnsi(In begin, In end, Out output, char replacement = 0, const std::locale& locale = std::locale());

	// Convert an UTF-8 characters range to wide characters
	template <typename In, typename Out>
	static Out toWide(In begin, In end, Out output, wchar_t replacement = 0);

	// Convert an UTF-8 characters range to latin-1 (ISO-5589-1) characters
	template <typename In, typename Out>
	static Out toLatin1(In begin, In end, Out output, char replacement = 0);

	// Convert a UTF-8 characters range to UTF-8
	/// This functions does nothing more than a direct copy; it is defined only to provide the same interface as other specializations of the sf::Utf<> template, and allow generic code to be written on top of it.
	template <typename In, typename Out>
	static Out toUtf8(In begin, In end, Out output);

	// Convert a UTF-8 characters range to UTF-16
	template <typename In, typename Out>
	static Out toUtf16(In begin, In end, Out output);

	// Convert a UTF-8 characters range to UTF-32
	template <typename In, typename Out>
	static Out toUtf32(In begin, In end, Out output);
};

// Specialization of the Utf template for UTF-16
template <>
class Utf<16>
{
public:
	// Decode a single UTF-16 character
	// Decoding a character means finding its unique 32-bits code (called the codepoint) in the Unicode standard.
	template <typename In>
	static In decode(In begin, In end, std::uint32_t& output, std::uint32_t replacement = 0);

	// Encode a single UTF-16 character
	// Encoding a character means converting a unique 32-bits code (called the codepoint) in the target encoding, UTF-16.
	template <typename Out>
	static Out encode(std::uint32_t input, Out output, std::uint16_t replacement = 0);

	// Advance to the next UTF-16 character
	/// This function is necessary for multi-elements encodings, as a single character may use more than 1 storage element.
	template <typename In>
	static In next(In begin, In end);

	// Count the number of characters of a UTF-16 sequence
	// This function is necessary for multi-elements encodings, as a single character may use more than 1 storage element, thus the total size can be different from (begin - end).
	template <typename In>
	static std::size_t count(In begin, In end);

	/// Convert an ANSI characters range to UTF-16
	// The current global locale will be used by default, unless you pass a custom one in the \a locale parameter.
	template <typename In, typename Out>
	static Out fromAnsi(In begin, In end, Out output, const std::locale& locale = std::locale());

	// Convert a wide characters range to UTF-16
	template <typename In, typename Out>
	static Out fromWide(In begin, In end, Out output);

	// Convert a latin-1 (ISO-5589-1) characters range to UTF-16
	template <typename In, typename Out>
	static Out fromLatin1(In begin, In end, Out output);

	// Convert an UTF-16 characters range to ANSI characters
	// The current global locale will be used by default, unless you pass a custom one in the \a locale parameter.
	template <typename In, typename Out>
	static Out toAnsi(In begin, In end, Out output, char replacement = 0, const std::locale& locale = std::locale());

	// Convert an UTF-16 characters range to wide characters
	template <typename In, typename Out>
	static Out toWide(In begin, In end, Out output, wchar_t replacement = 0);

	// Convert an UTF-16 characters range to latin-1 (ISO-5589-1) characters
	template <typename In, typename Out>
	static Out toLatin1(In begin, In end, Out output, char replacement = 0);

	// Convert a UTF-16 characters range to UTF-8
	template <typename In, typename Out>
	static Out toUtf8(In begin, In end, Out output);

	// Convert a UTF-16 characters range to UTF-16
	/// This functions does nothing more than a direct copy; it is defined only to provide the same interface as other specializations of the sf::Utf<> template, and allow generic code to be written on top of it.
	template <typename In, typename Out>
	static Out toUtf16(In begin, In end, Out output);

	// Convert a UTF-16 characters range to UTF-32
	template <typename In, typename Out>
	static Out toUtf32(In begin, In end, Out output);
};

// Specialization of the Utf template for UTF-32
template <>
class Utf<32>
{
public:
	// Decode a single UTF-32 character
	// Decoding a character means finding its unique 32-bits code (called the codepoint) in the Unicode standard. For UTF-32, the character value is the same as the codepoint.
	template <typename In>
	static In decode(In begin, In end, std::uint32_t& output, std::uint32_t replacement = 0);

	// Encode a single UTF-32 character
	// Encoding a character means converting a unique 32-bits code (called the codepoint) in the target encoding, UTF-32. For UTF-32, the codepoint is the same as the character value.
	template <typename Out>
	static Out encode(std::uint32_t input, Out output, std::uint32_t replacement = 0);

	// Advance to the next UTF-32 character
	// This function is trivial for UTF-32, which can store every character in a single storage element.
	template <typename In>
	static In next(In begin, In end);

	// Count the number of characters of a UTF-32 sequence
	// This function is trivial for UTF-32, which can store every character in a single storage element.
	template <typename In>
	static std::size_t count(In begin, In end);

	// Convert an ANSI characters range to UTF-32
	// The current global locale will be used by default, unless you pass a custom one in the \a locale parameter.
	template <typename In, typename Out>
	static Out fromAnsi(In begin, In end, Out output, const std::locale& locale = std::locale());

	// Convert a wide characters range to UTF-32
	template <typename In, typename Out>
	static Out fromWide(In begin, In end, Out output);

	// Convert a latin-1 (ISO-5589-1) characters range to UTF-32
	template <typename In, typename Out>
	static Out fromLatin1(In begin, In end, Out output);

	// Convert an UTF-32 characters range to ANSI characters
	// The current global locale will be used by default, unless you pass a custom one in the \a locale parameter.
	template <typename In, typename Out>
	static Out toAnsi(In begin, In end, Out output, char replacement = 0, const std::locale& locale = std::locale());

	// Convert an UTF-32 characters range to wide characters
	template <typename In, typename Out>
	static Out toWide(In begin, In end, Out output, wchar_t replacement = 0);

	// Convert an UTF-16 characters range to latin-1 (ISO-5589-1) characters
	template <typename In, typename Out>
	static Out toLatin1(In begin, In end, Out output, char replacement = 0);

	// Convert a UTF-32 characters range to UTF-8
	template <typename In, typename Out>
	static Out toUtf8(In begin, In end, Out output);

	// Convert a UTF-32 characters range to UTF-16
	template <typename In, typename Out>
	static Out toUtf16(In begin, In end, Out output);

	// Convert a UTF-32 characters range to UTF-32
	// This functions does nothing more than a direct copy; it is defined only to provide the same interface as other specializations of the Utf<> template, and allow generic code to be written on top of it.
	template <typename In, typename Out>
	static Out toUtf32(In begin, In end, Out output);

	// Decode a single ANSI character to UTF-32
	/// This function does not exist in other specializations of Utf<>, it is defined for convenience (it is used by several other conversion functions).
	template <typename In>
	static std::uint32_t decodeAnsi(In input, const std::locale& locale = std::locale());

	// Decode a single wide character to UTF-32
	/// This function does not exist in other specializations of Utf<>, it is defined for convenience (it is used by several other conversion functions).
	template <typename In>
	static std::uint32_t decodeWide(In input);

	// Encode a single UTF-32 character to ANSI
	// This function does not exist in other specializations of Utf<>, it is defined for convenience (it is used by several other conversion functions).
	template <typename Out>
	static Out encodeAnsi(std::uint32_t codepoint, Out output, char replacement = 0, const std::locale& locale = std::locale());

	// Encode a single UTF-32 character to wide
	// This function does not exist in other specializations of Utf<>, it is defined for convenience (it is used by several other conversion functions).
	template <typename Out>
	static Out encodeWide(std::uint32_t codepoint, Out output, wchar_t replacement = 0);
};

// Make type aliases to get rid of the template syntax
using Utf8 = Utf<8>;
using Utf16 = Utf<16>;
using Utf32 = Utf<32>;

template <typename In>
In Utf<8>::decode(In begin, In end, std::uint32_t& output, std::uint32_t replacement)
{
	// clang-format off
	// Some useful precomputed data
	static constexpr int trailing[256] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
	};

	static constexpr std::uint32_t offsets[6] =
	{
		0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080
	};
	// clang-format on

	// decode the character
	const int trailingBytes = trailing[static_cast<std::uint8_t>(*begin)];
	if (trailingBytes < std::distance(begin, end))
	{
		output = 0;

		// clang-format off
		switch (trailingBytes)
		{
		case 5: output += static_cast<std::uint8_t>(*begin++); output <<= 6; [[fallthrough]];
		case 4: output += static_cast<std::uint8_t>(*begin++); output <<= 6; [[fallthrough]];
		case 3: output += static_cast<std::uint8_t>(*begin++); output <<= 6; [[fallthrough]];
		case 2: output += static_cast<std::uint8_t>(*begin++); output <<= 6; [[fallthrough]];
		case 1: output += static_cast<std::uint8_t>(*begin++); output <<= 6; [[fallthrough]];
		case 0: output += static_cast<std::uint8_t>(*begin++);
		}
		// clang-format on

		output -= offsets[trailingBytes];
	}
	else
	{
		// Incomplete character
		begin = end;
		output = replacement;
	}

	return begin;
}

template <typename Out>
Out Utf<8>::encode(std::uint32_t input, Out output, std::uint8_t replacement)
{
	// Some useful precomputed data
	static constexpr std::uint8_t firstBytes[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	// encode the character
	if ((input > 0x0010FFFF) || ((input >= 0xD800) && (input <= 0xDBFF)))
	{
		// Invalid character
		if (replacement)
			*output++ = static_cast<typename Out::container_type::value_type>(replacement);
	}
	else
	{
		// Valid character

		// Get the number of bytes to write
		std::size_t bytestoWrite = 1;

		// clang-format off
		if (input < 0x80)       bytestoWrite = 1;
		else if (input < 0x800)      bytestoWrite = 2;
		else if (input < 0x10000)    bytestoWrite = 3;
		else if (input <= 0x0010FFFF) bytestoWrite = 4;
		// clang-format on

		// Extract the bytes to write
		std::byte bytes[4];

		// clang-format off
		switch (bytestoWrite)
		{
		case 4: bytes[3] = static_cast<std::byte>((input | 0x80) & 0xBF); input >>= 6; [[fallthrough]];
		case 3: bytes[2] = static_cast<std::byte>((input | 0x80) & 0xBF); input >>= 6; [[fallthrough]];
		case 2: bytes[1] = static_cast<std::byte>((input | 0x80) & 0xBF); input >>= 6; [[fallthrough]];
		case 1: bytes[0] = static_cast<std::byte> (input | firstBytes[bytestoWrite]);
		}
		// clang-format on

		// Add them to the output
		output = priv::copy(bytes, bytes + bytestoWrite, output);
	}

	return output;
}

template <typename In>
In Utf<8>::next(In begin, In end)
{
	std::uint32_t codepoint = 0;
	return decode(begin, end, codepoint);
}

template <typename In>
std::size_t Utf<8>::count(In begin, In end)
{
	std::size_t length = 0;
	while (begin < end)
	{
		begin = next(begin, end);
		++length;
	}

	return length;
}

template <typename In, typename Out>
Out Utf<8>::fromAnsi(In begin, In end, Out output, const std::locale& locale)
{
	while (begin < end)
	{
		const std::uint32_t codepoint = Utf<32>::decodeAnsi(*begin++, locale);
		output = encode(codepoint, output);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<8>::fromWide(In begin, In end, Out output)
{
	while (begin < end)
	{
		std::uint32_t codepoint = Utf<32>::decodeWide(*begin++);
		output = encode(codepoint, output);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<8>::fromLatin1(In begin, In end, Out output)
{
	// Latin-1 is directly compatible with Unicode encodings, and can thus be treated as (a sub-range of) UTF-32
	while (begin < end)
		output = encode(*begin++, output);

	return output;
}

template <typename In, typename Out>
Out Utf<8>::toAnsi(In begin, In end, Out output, char replacement, const std::locale& locale)
{
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		output = Utf<32>::encodeAnsi(codepoint, output, replacement, locale);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<8>::toWide(In begin, In end, Out output, wchar_t replacement)
{
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		output = Utf<32>::encodeWide(codepoint, output, replacement);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<8>::toLatin1(In begin, In end, Out output, char replacement)
{
	// Latin-1 is directly compatible with Unicode encodings, and can thus be treated as (a sub-range of) UTF-32
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		*output++ = codepoint < 256 ? static_cast<char>(codepoint) : replacement;
	}

	return output;
}

template <typename In, typename Out>
Out Utf<8>::toUtf8(In begin, In end, Out output)
{
	return priv::copy(begin, end, output);
}

template <typename In, typename Out>
Out Utf<8>::toUtf16(In begin, In end, Out output)
{
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		output = Utf<16>::encode(codepoint, output);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<8>::toUtf32(In begin, In end, Out output)
{
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		*output++ = codepoint;
	}

	return output;
}

template <typename In>
In Utf<16>::decode(In begin, In end, std::uint32_t& output, std::uint32_t replacement)
{
	const std::uint16_t first = *begin++;

	// If it's a surrogate pair, first convert to a single UTF-32 character
	if ((first >= 0xD800) && (first <= 0xDBFF))
	{
		if (begin < end)
		{
			const std::uint32_t second = *begin++;
			if ((second >= 0xDC00) && (second <= 0xDFFF))
			{
				// The second element is valid: convert the two elements to a UTF-32 character
				output = ((first - 0xD800u) << 10) + (second - 0xDC00) + 0x0010000;
			}
			else
			{
				// Invalid character
				output = replacement;
			}
		}
		else
		{
			// Invalid character
			begin = end;
			output = replacement;
		}
	}
	else
	{
		// We can make a direct copy
		output = first;
	}

	return begin;
}

template <typename Out>
Out Utf<16>::encode(std::uint32_t input, Out output, std::uint16_t replacement)
{
	if (input <= 0xFFFF)
	{
		// The character can be copied directly, we just need to check if it's in the valid range
		if ((input >= 0xD800) && (input <= 0xDFFF))
		{
			// Invalid character (this range is reserved)
			if (replacement)
				*output++ = replacement;
		}
		else
		{
			// Valid character directly convertible to a single UTF-16 character
			*output++ = static_cast<std::uint16_t>(input);
		}
	}
	else if (input > 0x0010FFFF)
	{
		// Invalid character (greater than the maximum Unicode value)
		if (replacement)
			*output++ = replacement;
	}
	else
	{
		// The input character will be converted to two UTF-16 elements
		input -= 0x0010000;
		*output++ = static_cast<std::uint16_t>((input >> 10) + 0xD800);
		*output++ = static_cast<std::uint16_t>((input & 0x3FFUL) + 0xDC00);
	}

	return output;
}

template <typename In>
In Utf<16>::next(In begin, In end)
{
	std::uint32_t codepoint = 0;
	return decode(begin, end, codepoint);
}

template <typename In>
std::size_t Utf<16>::count(In begin, In end)
{
	std::size_t length = 0;
	while (begin < end)
	{
		begin = next(begin, end);
		++length;
	}

	return length;
}

template <typename In, typename Out>
Out Utf<16>::fromAnsi(In begin, In end, Out output, const std::locale& locale)
{
	while (begin < end)
	{
		std::uint32_t codepoint = Utf<32>::decodeAnsi(*begin++, locale);
		output = encode(codepoint, output);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<16>::fromWide(In begin, In end, Out output)
{
	while (begin < end)
	{
		std::uint32_t codepoint = Utf<32>::decodeWide(*begin++);
		output = encode(codepoint, output);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<16>::fromLatin1(In begin, In end, Out output)
{
	// Latin-1 is directly compatible with Unicode encodings, and can thus be treated as (a sub-range of) UTF-32
	return priv::copy(begin, end, output);
}

template <typename In, typename Out>
Out Utf<16>::toAnsi(In begin, In end, Out output, char replacement, const std::locale& locale)
{
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		output = Utf<32>::encodeAnsi(codepoint, output, replacement, locale);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<16>::toWide(In begin, In end, Out output, wchar_t replacement)
{
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		output = Utf<32>::encodeWide(codepoint, output, replacement);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<16>::toLatin1(In begin, In end, Out output, char replacement)
{
	// Latin-1 is directly compatible with Unicode encodings, and can thus be treated as (a sub-range of) UTF-32
	while (begin < end)
	{
		*output++ = *begin < 256 ? static_cast<char>(*begin) : replacement;
		++begin;
	}

	return output;
}

template <typename In, typename Out>
Out Utf<16>::toUtf8(In begin, In end, Out output)
{
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		output = Utf<8>::encode(codepoint, output);
	}

	return output;
}

template <typename In, typename Out>
Out Utf<16>::toUtf16(In begin, In end, Out output)
{
	return priv::copy(begin, end, output);
}

template <typename In, typename Out>
Out Utf<16>::toUtf32(In begin, In end, Out output)
{
	while (begin < end)
	{
		std::uint32_t codepoint = 0;
		begin = decode(begin, end, codepoint);
		*output++ = codepoint;
	}

	return output;
}

template <typename In>
In Utf<32>::decode(In begin, In /*end*/, std::uint32_t& output, std::uint32_t /*replacement*/)
{
	output = *begin++;
	return begin;
}

template <typename Out>
Out Utf<32>::encode(std::uint32_t input, Out output, std::uint32_t /*replacement*/)
{
	*output++ = input;
	return output;
}

template <typename In>
In Utf<32>::next(In begin, In /*end*/)
{
	return ++begin;
}

template <typename In>
std::size_t Utf<32>::count(In begin, In end)
{
	return begin - end;
}

template <typename In, typename Out>
Out Utf<32>::fromAnsi(In begin, In end, Out output, const std::locale& locale)
{
	while (begin < end)
		*output++ = decodeAnsi(*begin++, locale);

	return output;
}

template <typename In, typename Out>
Out Utf<32>::fromWide(In begin, In end, Out output)
{
	while (begin < end)
		*output++ = decodeWide(*begin++);

	return output;
}

template <typename In, typename Out>
Out Utf<32>::fromLatin1(In begin, In end, Out output)
{
	// Latin-1 is directly compatible with Unicode encodings, and can thus be treated as (a sub-range of) UTF-32
	return priv::copy(begin, end, output);
}

template <typename In, typename Out>
Out Utf<32>::toAnsi(In begin, In end, Out output, char replacement, const std::locale& locale)
{
	while (begin < end)
		output = encodeAnsi(*begin++, output, replacement, locale);

	return output;
}

template <typename In, typename Out>
Out Utf<32>::toWide(In begin, In end, Out output, wchar_t replacement)
{
	while (begin < end)
		output = encodeWide(*begin++, output, replacement);

	return output;
}

template <typename In, typename Out>
Out Utf<32>::toLatin1(In begin, In end, Out output, char replacement)
{
	// Latin-1 is directly compatible with Unicode encodings, and can thus be treated as (a sub-range of) UTF-32
	while (begin < end)
	{
		*output++ = *begin < 256 ? static_cast<char>(*begin) : replacement;
		++begin;
	}

	return output;
}

template <typename In, typename Out>
Out Utf<32>::toUtf8(In begin, In end, Out output)
{
	while (begin < end)
		output = Utf<8>::encode(*begin++, output);

	return output;
}

template <typename In, typename Out>
Out Utf<32>::toUtf16(In begin, In end, Out output)
{
	while (begin < end)
		output = Utf<16>::encode(*begin++, output);

	return output;
}

template <typename In, typename Out>
Out Utf<32>::toUtf32(In begin, In end, Out output)
{
	return priv::copy(begin, end, output);
}

template <typename In>
std::uint32_t Utf<32>::decodeAnsi(In input, [[maybe_unused]] const std::locale& locale)
{
	// Get the facet of the locale which deals with character conversion
	const auto& facet = std::use_facet<std::ctype<wchar_t>>(locale);

	// Use the facet to convert each character of the input string
	return static_cast<std::uint32_t>(facet.widen(input));
}

template <typename In>
std::uint32_t Utf<32>::decodeWide(In input)
{
	// The encoding of wide characters is not well defined and is left to the system;
	// however we can safely assume that it is UCS-2 on Windows and
	// UCS-4 on Unix systems.
	// In both cases, a simple copy is enough (UCS-2 is a subset of UCS-4,
	// and UCS-4 *is* UTF-32).

	return static_cast<std::uint32_t>(input);
}

template <typename Out>
Out Utf<32>::encodeAnsi(std::uint32_t codepoint, Out output, char replacement, [[maybe_unused]] const std::locale& locale)
{
	// Get the facet of the locale which deals with character conversion
	const auto& facet = std::use_facet<std::ctype<wchar_t>>(locale);

	// Use the facet to convert each character of the input string
	*output++ = facet.narrow(static_cast<wchar_t>(codepoint), replacement);

	return output;
}

template <typename Out>
Out Utf<32>::encodeWide(std::uint32_t codepoint, Out output, wchar_t replacement)
{
	// The encoding of wide characters is not well defined and is left to the system;
	// however we can safely assume that it is UCS-2 on Windows and
	// UCS-4 on Unix systems.
	// For UCS-2 we need to check if the source characters fits in (UCS-2 is a subset of UCS-4).
	// For UCS-4 we can do a direct copy (UCS-4 *is* UTF-32).

	switch (sizeof(wchar_t))
	{
	case 4:
	{
		*output++ = static_cast<wchar_t>(codepoint);
		break;
	}

	default:
	{
		if ((codepoint <= 0xFFFF) && ((codepoint < 0xD800) || (codepoint > 0xDFFF)))
		{
			*output++ = static_cast<wchar_t>(codepoint);
		}
		else if (replacement)
		{
			*output++ = replacement;
		}
		break;
	}
	}

	return output;
}