#pragma once

#include "Utf.h"

// String s;
// std::string s1 = s;  // automatically converted to ANSI string
// std::wstring s2 = s; // automatically converted to wide string
// s = "hello";         // automatically converted from ANSI string
// s = L"hello";        // automatically converted from wide string
// s += 'a';            // automatically converted from ANSI string
// s += L'a';           // automatically converted from wide string

class String
{
public:
	using Iterator = std::u32string::iterator;            // Iterator type
	using ConstIterator = std::u32string::const_iterator; // Read-only iterator type

	static inline const std::size_t InvalidPos{ std::u32string::npos };

	String() = default;
	String(char ansiChar, const std::locale& locale = std::locale());
	String(wchar_t wideChar);
	String(char32_t utf32Char);
	String(const char* ansiString, const std::locale& locale = std::locale());
	String(const std::string& ansiString, const std::locale& locale = std::locale());
	String(const wchar_t* wideString);
	String(const std::wstring& wideString);
	String(const char32_t* utf32String);
	String(std::u32string utf32String);

	template <typename T>
	static String FromUtf8(T begin, T end);  // Create a new String from a UTF-8 encoded string
	template <typename T>
	static String FromUtf16(T begin, T end); // Create a new String from a UTF-16 encoded string
	template <typename T>
	static String FromUtf32(T begin, T end); // Create a new String from a UTF-32 encoded string

	operator std::string() const;
	operator std::wstring() const;

	// Convert the Unicode string to an ANSI string
	[[nodiscard]] std::string ToAnsiString(const std::locale& locale = std::locale()) const;
	// Convert the Unicode string to a wide string
	[[nodiscard]] std::wstring ToWideString() const;

	std::u8string ToUtf8() const;
	std::u16string ToUtf16() const;
	std::u32string ToUtf32() const;

	String& operator+=(const String& right);

	char32_t operator[](std::size_t index) const;
	char32_t& operator[](std::size_t index);

	void clear();

	std::size_t getSize() const;

	bool isEmpty() const;

	void erase(std::size_t position, std::size_t count = 1);

	void insert(std::size_t position, const String& str);

	[[nodiscard]] std::size_t find(const String& str, std::size_t start = 0) const;

	void replace(std::size_t position, std::size_t length, const String& replaceWith);
	void replace(const String& searchFor, const String& replaceWith);

	[[nodiscard]] String substring(std::size_t position, std::size_t length = InvalidPos) const;

	const char32_t* getData() const;

	Iterator begin();
	ConstIterator begin() const;
	Iterator end();
	ConstIterator end() const;

private:
	friend bool operator==(const String& left, const String& right);
	friend bool operator<(const String& left, const String& right);

	std::u32string m_string;
};

bool operator==(const String& left, const String& right);
bool operator!=(const String& left, const String& right);

String operator+(const String& left, const String& right);

template <typename T>
String String::FromUtf8(T begin, T end)
{
	String string;
	Utf8::toUtf32(begin, end, std::back_inserter(string.m_string));
	return string;
}

template <typename T>
String String::FromUtf16(T begin, T end)
{
	String string;
	Utf16::toUtf32(begin, end, std::back_inserter(string.m_string));
	return string;
}

template <typename T>
String String::FromUtf32(T begin, T end)
{
	String string;
	string.m_string.assign(begin, end);
	return string;
}
