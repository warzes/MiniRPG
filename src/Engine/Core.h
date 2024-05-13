#pragma once

//==============================================================================
// MACROS
//==============================================================================

#define PLATFORM_LINUX 0
#define PLATFORM_BSD 0
#define PLATFORM_WINDOWS 0
#define PLATFORM_EMSCRIPTEN 0
#define PLATFORM_ANDROID 0

#define PLATFORM_DESKTOP 0

#if defined(linux) || defined(__linux) || defined(__linux__)
#	undef  PLATFORM_LINUX
#	define PLATFORM_LINUX 1
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#	undef  PLATFORM_BSD
#	define PLATFORM_BSD 1
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WINDOWS)
#	undef  PLATFORM_WINDOWS
#	define PLATFORM_WINDOWS 1
#elif defined(__EMSCRIPTEN__)
#	undef  PLATFORM_EMSCRIPTEN
#	define PLATFORM_EMSCRIPTEN 1
#elif defined(ANDROID) || defined(__ANDROID__)
#	undef  PLATFORM_ANDROID
#	define PLATFORM_ANDROID 1
#else
#	error "Unknown Platform"
#endif

#if PLATFORM_LINUX || PLATFORM_BSD || PLATFORM_WINDOWS
#	undef  PLATFORM_DESKTOP
#	define PLATFORM_DESKTOP 1
#endif

// Turn argument to string constant:
// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html#Stringizing
#define SE_STRINGIZE( _n )   SE_STRINGIZE_2( _n )
#define SE_STRINGIZE_2( _n ) #_n

#if defined(_MSC_VER)
#	define TODO( _msg )  __pragma(message("" __FILE__ "(" SE_STRINGIZE(__LINE__) "): TODO: " _msg))
#else
#	define TODO( _msg )
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