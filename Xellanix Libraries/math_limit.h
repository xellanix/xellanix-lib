#ifndef XELLANIX_MATH_LIMIT_H
#define XELLANIX_MATH_LIMIT_H
#endif // !XELLANIX_MATH_LIMIT_H

#ifndef XELLANIX_MATH_TRAIT_H
#include "math_trait.h"
#endif // !XELLANIX_MATH_TRAIT_H

#include <limits>

namespace xellanix::type::limit
{
	#pragma region NUMERIC LIMITS

	template<typename T, xellanix::type::trait::enable_if_arithm<T> = true>
	inline constexpr decltype(auto) nmin()
	{
		return (std::numeric_limits<T>::min)();
	}

	template<typename T, xellanix::type::trait::enable_if_arithm<T> = true>
	inline constexpr decltype(auto) nmax()
	{
		return (std::numeric_limits<T>::max)();
	}

	template<typename T, xellanix::type::trait::enable_if_arithm<T> = true>
	inline constexpr decltype(auto) limit_range()
	{
		return std::pair{ nmin<T>(), nmax<T>() };
	}

	template<typename LT, typename RT>
	inline constexpr auto is_same_limit()
	{
		const auto lhs = xellanix::type::limit::limit_range<LT>();
		const auto rhs = xellanix::type::limit::limit_range<RT>();

		return lhs.first == rhs.first && lhs.second == rhs.second;
	}

	#pragma endregion
}