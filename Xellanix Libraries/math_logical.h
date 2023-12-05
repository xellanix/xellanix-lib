#ifndef XELLANIX_MATH_LOGICAL_H
#define XELLANIX_MATH_LOGICAL_H
#endif // !XELLANIX_MATH_LOGICAL_H

#ifndef XELLANIX_MATH_OP_H
#include "math_op.h"
#endif // !XELLANIX_MATH_OP_H

namespace xellanix::math
{
	template<typename Floating, xellanix::type::helper::enable_if_floating_point<Floating> = true>
	inline constexpr bool is_negative(Floating value)
	{
		constexpr auto zero = Floating{ 0.0 };

		constexpr bool eql = [](auto left, auto right)
		{
			if (std::_Is_inf(left) || std::_Is_nan(left))
			{
				return false;
			}

			const auto factor = (std::max)({ Floating{ 1.0 }, xellanix::math::abs(left), right });
			return xellanix::math::abs(left - right) < std::numeric_limits<Floating>::epsilon() * factor;
		};

		if (!eql(value, zero))
		{
			return value < zero;
		}
		return false;
	}

	template<typename Integral, xellanix::type::helper::enable_if_integral<Integral> = true>
	inline constexpr bool is_negative(Integral value)
	{
		return value < Integral{ 0 };
	}

	template<typename T, xellanix::type::helper::enable_if_arithm<T> = true>
	inline constexpr bool is_positive(T value)
	{
		return !is_negative(value);
	}

	template <typename Integral, xellanix::type::helper::enable_if_integral<Integral> = true>
	inline constexpr bool is_inf(const Integral value)
	{
		return false;
	}

	template <typename Floating, xellanix::type::helper::enable_if_floating_point<Floating> = true>
	inline constexpr bool is_inf(const Floating value)
	{
		return xellanix::math::detail::_is_inf(value);
	}

	template <typename Integral, xellanix::type::helper::enable_if_integral<Integral> = true>
	inline constexpr bool is_nan(const Integral value)
	{
		return false;
	}

	template <typename Floating, xellanix::type::helper::enable_if_floating_point<Floating> = true>
	inline constexpr bool is_nan(const Floating value)
	{
		return xellanix::math::detail::_is_nan(value);
	}

	// Is the division results integer or decimal
	template<typename L, typename R, std::enable_if_t<std::is_floating_point_v<L> || std::is_floating_point_v<R>, bool> = true>
	inline constexpr bool is_divres_int(L lhs, R rhs) noexcept
	{
		return false;
	}

	template<typename L, typename R, xellanix::type::helper::enable_if_integrals<L, R> = true>
	inline constexpr bool is_divres_int(L lhs, R rhs)
	{
		return xellanix::math::mod(lhs, rhs) == 0;
	}
}