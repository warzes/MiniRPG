#pragma once

//==============================================================================
// MACROS
//==============================================================================

// Turn argument to string constant:
// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html#Stringizing
#define SE_STRINGIZE( _n )   SE_STRINGIZE_2( _n )
#define SE_STRINGIZE_2( _n ) #_n

#if defined(_MSC_VER)
#	define TODO( _msg )  __pragma(message("" __FILE__ "(" SE_STRINGIZE(__LINE__) "): TODO: " _msg))
#else
#	define TODO( _msg )
#endif

#if defined(_WIN32)
#endif
#if defined(__linux__)
#endif
#if defined(__EMSCRIPTEN__)
#endif

//==============================================================================
// INLINE FUNC
//==============================================================================

// Implementation from "08/02/2015 Better array 'countof' implementation with C++ 11 (updated)" - https://www.g-truc.net/post-0708.html
template<typename T, size_t N>
[[nodiscard]] inline constexpr size_t Countof(T const (&)[N])
{
	return N;
}

//template<class T>
//inline constexpr void Swap(T& left, T& right) noexcept
//{
//	T tmp = std::move(left);
//	left = std::move(right);
//	right = std::move(tmp);
//}

template<typename Result, typename Original>
[[nodiscard]] inline constexpr Result FunctionCast(Original fptr)
{
	return reinterpret_cast<Result>(reinterpret_cast<void*>(fptr));
}

//==============================================================================
// CORE FUNC
//==============================================================================

void Exit();