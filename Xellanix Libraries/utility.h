#pragma once

#ifndef XELLANIX_UTILITY_H
#define XELLANIX_UTILITY_H
#endif // !XELLANIX_UTILITY_H

#include <array>

#define GCC_COMPILER (defined __GNUG__ && !defined __clang__)
#define MSVC_COMPILER (defined _MSC_VER)

namespace xellanix::utility
{
	// cast from Source to Target type
	// Source and Target type are same, so just forward it
	template<typename T, typename S, std::enable_if_t<std::is_same_v<T, S>, bool> = true>
	inline constexpr std::enable_if_t<std::is_arithmetic_v<T>, T> cast_to(S const value)
	{
		return value;
	}

	// cast from Source to Target type
	// Source and Target type are same, so cast it
	template<typename T, typename S, std::enable_if_t<!std::is_same_v<T, S>, bool> = true>
	inline constexpr std::enable_if_t<std::is_arithmetic_v<T>, T> cast_to(S const value)
	{
		return static_cast<T>(value);
	}

	template<typename T,
		typename U = std::conditional_t<std::is_same_v<std::remove_cv_t<T>, bool>, char, typename std::remove_cv<T>::template _Apply<std::_Make_signed1>>,
		std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, bool> = true>
	inline constexpr U make_signed(T n)
	{
		return xellanix::utility::cast_to<U>(n);
	}
	
	template<typename T,
		typename U = std::conditional_t<std::is_same_v<std::remove_cv_t<T>, bool>, unsigned char, typename std::remove_cv<T>::template _Apply<std::_Make_unsigned1>>,
		std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, bool> = true>
	inline constexpr U make_unsigned(T n)
	{
		return xellanix::utility::cast_to<U>(n);
	}

	template<typename... T>
	inline constexpr std::array<std::common_type_t<T...>, sizeof...(T)> make_array(T&&...t)
	{
		using first_t = typename std::common_type_t<T...>;
		return { std::forward<first_t>(t)... };
	}
}
