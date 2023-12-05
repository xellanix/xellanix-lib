#pragma once

#ifndef XELLANIX_COMPARE_HELPER_H
#define XELLANIX_COMPARE_HELPER_H
#endif // !XELLANIX_COMPARE_HELPER_H

#ifndef XELLANIX_MATH_OP_H
#include "math_op.h"
#endif // !XELLANIX_MATH_OP_H

namespace xellanix::math
{
	namespace xth = xellanix::type::helper;

	#pragma region EQUAL TO

	// Compares two same "floating types" whether it is 
	// "equal to"
	template<typename T>
	inline constexpr xth::enable_if_floating_point<T>
		equal(const T left, const T right)
	{
		if (xellanix::math::detail::_is_inf(left) && xellanix::math::detail::_is_inf(right))
		{
			return left == right;
		}
		else if (xellanix::math::detail::_is_nan(left) != xellanix::math::detail::_is_nan(right))
		{
			return false;
		}
		else if (xellanix::math::detail::_is_nan(left) && xellanix::math::detail::_is_nan(right))
		{
			return true;
		}

		const auto factor = (std::max)({ T{ 1.0 }, xellanix::math::abs(left), xellanix::math::abs(right) });
		return xellanix::math::abs(left - right) < std::numeric_limits<T>::epsilon() * factor;
	}
	
	// Compares two arithmetic types that based on "floating type(s)" whether it is 
	// "equal to"
	template<typename T1, typename T2>
	inline constexpr std::enable_if_t<(std::is_floating_point_v<T1> || std::is_floating_point_v<T2>) && (xth::detail::is_arithmetic<T1, T2>::value), bool>
		equal(const T1 leftRaw, const T2 rightRaw)
	{
		using floating_type = typename std::common_type_t<T1, T2>;
		const auto left = xellanix::utility::cast_to<floating_type>(leftRaw);
		const auto right = xellanix::utility::cast_to<floating_type>(rightRaw);

		return equal(left, right);
	}

	// Compares two "integers" whether it is
	// "equal to"
	template<typename T1, typename T2>
	inline constexpr xth::enable_if_integrals<T1, T2>
		equal(const T1 leftRaw, const T2 rightRaw)
	{
		using LT = typename std::conditional_t<type::helper::is_same_r_v<T1, bool>, unsigned char, T1>;
		using RT = typename std::conditional_t<type::helper::is_same_r_v<T2, bool>, unsigned char, T2>;

		const auto left = xellanix::utility::cast_to<LT>(leftRaw);
		const auto right = xellanix::utility::cast_to<RT>(rightRaw);

		return left == right;
	}

	#pragma endregion

	#pragma region NOT EQUAL TO

	// Compares two "arithmetic types" whether it is 
	// "not equal to"
	template<typename T1, typename T2>
	inline constexpr xth::enable_if_arithms<T1, T2>
		not_equal(const T1 left, const T2 right)
	{
		return !equal(left, right);
	}

	#pragma endregion

	#pragma region LESS THAN

	// Compares two "integers" whether it is 
	// "less than"
	template<typename T1, typename T2>
	inline constexpr xth::enable_if_integrals<T1, T2>
		less(const T1 leftRaw, const T2 rightRaw)
	{
		using LT = typename std::conditional_t<type::helper::is_same_r_v<T1, bool>, unsigned char, T1>;
		using RT = typename std::conditional_t<type::helper::is_same_r_v<T2, bool>, unsigned char, T2>;

		const auto left = xellanix::utility::cast_to<LT>(leftRaw);
		const auto right = xellanix::utility::cast_to<RT>(rightRaw);

		if constexpr (std::is_signed_v<LT> == std::is_signed_v<RT>)
		{
			// Two Signed or Unsigned Type
			return left < right;
		}
		else if constexpr ((std::is_signed_v<LT> && std::is_unsigned_v<RT>))
		{
			// Signed and Unsigned Type
			if (left < 0) return true;
			else
			{
				return xellanix::utility::make_unsigned(left) < right;
			}
		}
		else
		{
			// Unsigned and Signed Type
			if (right <= 0) return false;
			else
			{
				return left < xellanix::utility::make_unsigned(right);
			}
		}
	}

	// Compares two "floating types" whether it is 
	// "less than"
	template<typename T>
	inline constexpr xth::enable_if_floating_point<T>
		less(const T left, const T right)
	{
		if (!equal(left, right))
		{
			return left < right;
		}
		return false;
	}

	// Compares two arithmetic types that based on "floating type(s)" whether it is 
	// "less than"
	template<typename T1, typename T2>
	inline constexpr std::enable_if_t<(std::is_floating_point_v<T1> || std::is_floating_point_v<T2>) && (xth::detail::is_arithmetic<T1, T2>::value), bool>
		less(const T1 leftRaw, const T2 rightRaw)
	{
		using floating_type = typename std::common_type_t<T1, T2>;
		const auto left = xellanix::utility::cast_to<floating_type>(leftRaw);
		const auto right = xellanix::utility::cast_to<floating_type>(rightRaw);

		return less(left, right);
	}

	#pragma endregion

	#pragma region LESS THAN OR EQUAL

	// Compares two "integers" whether it is 
	// "less than or equal to"
	template<typename T1, typename T2>
	inline constexpr xth::enable_if_integrals<T1, T2>
		less_eq(const T1 leftRaw, const T2 rightRaw)
	{
		using LT = typename std::conditional_t<type::helper::is_same_r_v<T1, bool>, unsigned char, T1>;
		using RT = typename std::conditional_t<type::helper::is_same_r_v<T2, bool>, unsigned char, T2>;

		const auto left = xellanix::utility::cast_to<LT>(leftRaw);
		const auto right = xellanix::utility::cast_to<RT>(rightRaw);

		if constexpr (!(std::is_signed_v<LT> ^ std::is_signed_v<RT>))
		{
			// Two Signed or Unsigned Type
			return left <= right;
		}
		else if constexpr ((std::is_signed_v<LT> && std::is_unsigned_v<RT>))
		{
			// Signed and Unsigned Type
			if (left <= 0) return true;
			else
			{
				return xellanix::utility::make_unsigned(left) <= right;
			}
		}
		else
		{
			// Unsigned and Signed Type
			if (right < 0) return false;
			else
			{
				return left <= xellanix::utility::make_unsigned(right);
			}
		}
	}

	// Compares two "floating types" whether it is 
	// "less than or equal to"
	template<typename T>
	inline constexpr xth::enable_if_floating_point<T>
		less_eq(const T left, const T right)
	{
		if (equal(left, right))
		{
			return true;
		}
		return left < right;
	}

	// Compares two arithmetic types that based on "floating type(s)" whether it is 
	// "less than or equal to"
	template<typename T1, typename T2>
	inline constexpr std::enable_if_t<(std::is_floating_point_v<T1> || std::is_floating_point_v<T2>) && (xth::detail::is_arithmetic<T1, T2>::value), bool>
		less_eq(const T1 leftRaw, const T2 rightRaw)
	{
		using floating_type = typename std::common_type_t<T1, T2>;
		const auto left = xellanix::utility::cast_to<floating_type>(leftRaw);
		const auto right = xellanix::utility::cast_to<floating_type>(rightRaw);

		return less_eq(left, right);
	}

	#pragma endregion

	#pragma region GREATER THAN

	// Compares two "arithmetic types" whether it is 
	// "greater than"
	template<typename T1, typename T2>
	inline constexpr xth::enable_if_arithms<T1, T2>
		greater(const T1 left, const T2 right)
	{
		return !less_eq(left, right);
	}

	#pragma endregion

	#pragma region GREATER THAN OR EQUAL

	// Compares two "arithmetic types" whether it is 
	// "greater than or equal to"
	template<typename T1, typename T2>
	inline constexpr xth::enable_if_arithms<T1, T2>
		greater_eq(const T1 left, const T2 right)
	{
		return !less(left, right);
	}

	#pragma endregion
}