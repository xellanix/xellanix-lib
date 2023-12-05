#ifndef XELLANIX_MATH_OP_H
#define XELLANIX_MATH_OP_H
#endif // !XELLANIX_MATH_OP_H

#ifndef XELLANIX_NUMBER_HELPER_H
#include "number_helper.h"
#endif // !XELLANIX_UTILITY_H

// Functions:
// ipow, ceil, ceil2, floor, floor2
// is based on
// https://github.com/elbeno/constexpr/blob/master/src/include/cx_math.h
namespace xellanix::math::detail
{
	template <typename FP, std::enable_if_t<std::is_floating_point_v<FP>, bool> = true>
	inline constexpr FP ipow(FP x, int n)
	{
		return (n == 0) ? FP{ 1 } :
			n == 1 ? x :
			n > 1 ? ((n & 1) ? x * xellanix::math::detail::ipow(x, n - 1) : xellanix::math::detail::ipow(x, n / 2) * xellanix::math::detail::ipow(x, n / 2)) :
			FP{ 1 } / xellanix::math::detail::ipow(x, -n);
	}

	template <typename T>
	inline constexpr T floor2(T x, T guess, T inc)
	{
		return guess + inc <= x ? xellanix::math::detail::floor2(x, guess + inc, inc) :
			inc <= T{ 1 } ? guess : xellanix::math::detail::floor2(x, guess, inc / T{ 2 });
	}

	template <typename T>
	inline constexpr T floor(T x, T guess, T inc)
	{
		return
			inc < T{ 8 } ? xellanix::math::detail::floor2(x, guess, inc) :
			guess + inc <= x ? xellanix::math::detail::floor(x, guess + inc, inc) :
			guess + (inc / T{ 8 }) * T { 7 } <= x ? xellanix::math::detail::floor(x, guess + (inc / T{ 8 }) * T { 7 }, inc / T{ 8 }) :
			guess + (inc / T{ 8 }) * T { 6 } <= x ? xellanix::math::detail::floor(x, guess + (inc / T{ 8 }) * T { 6 }, inc / T{ 8 }) :
			guess + (inc / T{ 8 }) * T { 5 } <= x ? xellanix::math::detail::floor(x, guess + (inc / T{ 8 }) * T { 5 }, inc / T{ 8 }) :
			guess + (inc / T{ 8 }) * T { 4 } <= x ? xellanix::math::detail::floor(x, guess + (inc / T{ 8 }) * T { 4 }, inc / T{ 8 }) :
			guess + (inc / T{ 8 }) * T { 3 } <= x ? xellanix::math::detail::floor(x, guess + (inc / T{ 8 }) * T { 3 }, inc / T{ 8 }) :
			guess + (inc / T{ 8 }) * T { 2 } <= x ? xellanix::math::detail::floor(x, guess + (inc / T{ 8 }) * T { 2 }, inc / T{ 8 }) :
			guess + inc / T{ 8 } <= x ? xellanix::math::detail::floor(x, guess + inc / T{ 8 }, inc / T{ 8 }) :
			xellanix::math::detail::floor(x, guess, inc / T{ 8 });
	}

	template <typename T>
	inline constexpr T floorP(T x)
	{
		return x >= T{ 0 } ? xellanix::math::detail::floor(x, T{ 0.0 }, xellanix::math::detail::ipow(T{ 2.0 }, std::numeric_limits<T>::max_exponent - 1)) :
			std::numeric_limits<T>::quiet_NaN();
	}

	template <typename T>
	inline constexpr T ceil2(T x, T guess, T dec)
	{
		return guess - dec >= x ? xellanix::math::detail::ceil2(x, guess - dec, dec) :
			dec <= T{ 1 } ? guess : xellanix::math::detail::ceil2(x, guess, dec / T{ 2 });
	}

	template <typename T>
	inline constexpr T ceil(T x, T guess, T dec)
	{
		return
			dec < T{ 8 } ? xellanix::math::detail::ceil2(x, guess, dec) :
			guess - dec >= x ? xellanix::math::detail::ceil(x, guess - dec, dec) :
			guess - (dec / T{ 8 }) * T { 7 } >= x ? xellanix::math::detail::ceil(x, guess - (dec / T{ 8 }) * T { 7 }, dec / T{ 8 }) :
			guess - (dec / T{ 8 }) * T { 6 } >= x ? xellanix::math::detail::ceil(x, guess - (dec / T{ 8 }) * T { 6 }, dec / T{ 8 }) :
			guess - (dec / T{ 8 }) * T { 5 } >= x ? xellanix::math::detail::ceil(x, guess - (dec / T{ 8 }) * T { 5 }, dec / T{ 8 }) :
			guess - (dec / T{ 8 }) * T { 4 } >= x ? xellanix::math::detail::ceil(x, guess - (dec / T{ 8 }) * T { 4 }, dec / T{ 8 }) :
			guess - (dec / T{ 8 }) * T { 3 } >= x ? xellanix::math::detail::ceil(x, guess - (dec / T{ 8 }) * T { 3 }, dec / T{ 8 }) :
			guess - (dec / T{ 8 }) * T { 2 } >= x ? xellanix::math::detail::ceil(x, guess - (dec / T{ 8 }) * T { 2 }, dec / T{ 8 }) :
			guess - dec / T{ 8 } >= x ? xellanix::math::detail::ceil(x, guess - dec / T{ 8 }, dec / T{ 8 }) :
			xellanix::math::detail::ceil(x, guess, dec / T{ 8 });
	}

	template <typename T>
	inline constexpr T ceilP(T x)
	{
		constexpr auto ipowres = xellanix::math::detail::ipow(T{ 2.0 }, std::numeric_limits<T>::max_exponent - 1);

		return x >= T{ 0 } ? xellanix::math::detail::ceil(x, ipowres, ipowres) :
			std::numeric_limits<T>::quiet_NaN();
	}

	/// DERIVED FROM MSVC TYPE_TRAITS HEADER ///
	template <class _FloatingType>
	struct _Floating_type_traits;

	template <>
	struct _Floating_type_traits<float>
	{
		static constexpr int32_t _Mantissa_bits = 24; // FLT_MANT_DIG
		static constexpr int32_t _Exponent_bits = 8; // sizeof(float) * CHAR_BIT - FLT_MANT_DIG
		static constexpr int32_t _Maximum_binary_exponent = 127; // FLT_MAX_EXP - 1
		static constexpr int32_t _Minimum_binary_exponent = -126; // FLT_MIN_EXP - 1
		static constexpr int32_t _Exponent_bias = 127;
		static constexpr int32_t _Sign_shift = 31; // _Exponent_bits + _Mantissa_bits - 1
		static constexpr int32_t _Exponent_shift = 23; // _Mantissa_bits - 1

		using _Uint_type = uint32_t;

		static constexpr uint32_t _Exponent_mask = 0x000000FFu; // (1u << _Exponent_bits) - 1
		static constexpr uint32_t _Normal_mantissa_mask = 0x00FFFFFFu; // (1u << _Mantissa_bits) - 1
		static constexpr uint32_t _Denormal_mantissa_mask = 0x007FFFFFu; // (1u << (_Mantissa_bits - 1)) - 1
		static constexpr uint32_t _Special_nan_mantissa_mask = 0x00400000u; // 1u << (_Mantissa_bits - 2)
		static constexpr uint32_t _Shifted_sign_mask = 0x80000000u; // 1u << _Sign_shift
		static constexpr uint32_t _Shifted_exponent_mask = 0x7F800000u; // _Exponent_mask << _Exponent_shift
	};

	template <>
	struct _Floating_type_traits<double>
	{
		static constexpr int32_t _Mantissa_bits = 53; // DBL_MANT_DIG
		static constexpr int32_t _Exponent_bits = 11; // sizeof(double) * CHAR_BIT - DBL_MANT_DIG
		static constexpr int32_t _Maximum_binary_exponent = 1023; // DBL_MAX_EXP - 1
		static constexpr int32_t _Minimum_binary_exponent = -1022; // DBL_MIN_EXP - 1
		static constexpr int32_t _Exponent_bias = 1023;
		static constexpr int32_t _Sign_shift = 63; // _Exponent_bits + _Mantissa_bits - 1
		static constexpr int32_t _Exponent_shift = 52; // _Mantissa_bits - 1

		using _Uint_type = uint64_t;

		static constexpr uint64_t _Exponent_mask = 0x00000000000007FFu; // (1ULL << _Exponent_bits) - 1
		static constexpr uint64_t _Normal_mantissa_mask = 0x001FFFFFFFFFFFFFu; // (1ULL << _Mantissa_bits) - 1
		static constexpr uint64_t _Denormal_mantissa_mask = 0x000FFFFFFFFFFFFFu; // (1ULL << (_Mantissa_bits - 1)) - 1
		static constexpr uint64_t _Special_nan_mantissa_mask = 0x0008000000000000u; // 1ULL << (_Mantissa_bits - 2)
		static constexpr uint64_t _Shifted_sign_mask = 0x8000000000000000u; // 1ULL << _Sign_shift
		static constexpr uint64_t _Shifted_exponent_mask = 0x7FF0000000000000u; // _Exponent_mask << _Exponent_shift
	};

	template <>
	struct _Floating_type_traits<long double> : _Floating_type_traits<double> {};

	template <typename traits, typename FP, xellanix::type::helper::enable_if_floating_point<FP> = true>
	inline constexpr auto _Float_abs_bits(const FP fp)
	{
		using _Uint_type = typename traits::_Uint_type;

		const auto bits = __builtin_bit_cast(_Uint_type, fp);
		return bits & ~traits::_Shifted_sign_mask;
	}

	template <typename FP, xellanix::type::helper::enable_if_floating_point<FP> = true>
	inline constexpr bool _is_inf(const FP fp)
	{
		using _Traits = xellanix::math::detail::_Floating_type_traits<FP>;

		return xellanix::math::detail::_Float_abs_bits<_Traits>(fp) == _Traits::_Shifted_exponent_mask;
	}

	template <typename FP, xellanix::type::helper::enable_if_floating_point<FP> = true>
	inline constexpr bool _is_nan(const FP fp)
	{
		using _Traits = xellanix::math::detail::_Floating_type_traits<FP>;

		return xellanix::math::detail::_Float_abs_bits<_Traits>(fp) > _Traits::_Shifted_exponent_mask;
	}
}

namespace xellanix::math
{
	// constexpr absolute "integers"
	template<typename T, xellanix::type::helper::enable_if_integral<T> = true>
	inline constexpr std::make_unsigned_t<T> abs(T n)
	{
		return n < 0 ? 0ULL - n : n;
	}

	// constexpr absolute "floating types"
	template<typename T, xellanix::type::helper::enable_if_floating_point<T> = true>
	inline constexpr T abs(T n)
	{
		return n < 0 ? -n : n;
	}

	// constexpr ceil "integers"
	template<typename T, xellanix::type::helper::enable_if_integral<T> = true>
	inline constexpr T ceil(T x) noexcept
	{
		return x;
	}

	// constexpr ceil "floating types"
	template<typename T, xellanix::type::helper::enable_if_floating_point<T> = true>
	inline constexpr T ceil(T x)
	{
		return x < T{ 0 } ? -xellanix::math::detail::floorP(-x) : xellanix::math::detail::ceilP(x);
	}

	// constexpr floor "integers"
	template<typename T, xellanix::type::helper::enable_if_integral<T> = true>
	inline constexpr T floor(T x) noexcept
	{
		return x;
	}

	// constexpr floor "floating types"
	template<typename T, xellanix::type::helper::enable_if_floating_point<T> = true>
	inline constexpr T floor(T x)
	{
		return x < T{ 0 } ? -xellanix::math::detail::ceilP(-x) : xellanix::math::detail::floorP(x);
	}

	// constexpr truncate "integers"
	template<typename T, xellanix::type::helper::enable_if_integral<T> = true>
	inline constexpr T trunc(T x)
	{
		return x;
	}

	// constexpr truncate "floating types"
	template<typename T, xellanix::type::helper::enable_if_floating_point<T> = true>
	inline constexpr T trunc(T x)
	{
		return x >= T{ 0 } ? xellanix::math::floor(x) : xellanix::math::ceil(x);
	}

	// constexpr modulo "integers"
	template<typename T, xellanix::type::helper::enable_if_integral<T> = true>
	inline constexpr T mod(T number, T divisor)
	{
		return number % divisor;
	}

	// constexpr modulo "floating types"
	template<typename T, xellanix::type::helper::enable_if_floating_point<T> = true>
	inline constexpr T mod(T number, T divisor)
	{
		if (divisor == T{ 0.0 } || xellanix::math::detail::_is_inf(divisor) || xellanix::math::detail::_is_nan(divisor))
		{
			// Handle division by zero error if needed
			return std::numeric_limits<T>::quiet_NaN(); // Not-a-Number as an example
		}

		auto quotient = number / divisor;
		auto intPart = xellanix::math::trunc(quotient);
		return number - intPart * divisor;
	}

	// constexpr modulo "arithmetics"
	template<typename L, typename R, xellanix::type::helper::enable_if_arithms<L, R> = true>
	inline constexpr auto mod(L number, R divisor)
	{
		using ct = typename std::common_type_t<L, R>;

		return xellanix::math::mod(xellanix::utility::cast_to<ct>(number), xellanix::utility::cast_to<ct>(divisor));
	}

	// constexpr floored modulo "integers"
	template<typename T, xellanix::type::helper::enable_if_integral<T> = true>
	inline constexpr T mod_floor(T number, T divisor)
	{
		return ((number % divisor) + divisor) % divisor;
	}

	// constexpr floored modulo "floating types"
	template<typename T, xellanix::type::helper::enable_if_floating_point<T> = true>
	inline constexpr T mod_floor(T number, T divisor)
	{
		if (divisor == T{ 0.0 } || xellanix::math::detail::_is_inf(divisor) || xellanix::math::detail::_is_nan(divisor))
		{
			// Handle division by zero error if needed
			return std::numeric_limits<T>::quiet_NaN(); // Not-a-Number as an example
		}

		auto quotient = number / divisor;
		auto intPart = xellanix::math::floor(quotient);
		return number - intPart * divisor;
	}

	// constexpr floored modulo "arithmetics"
	template<typename L, typename R, xellanix::type::helper::enable_if_arithms<L, R> = true>
	inline constexpr auto mod_floor(L number, R divisor)
	{
		using ct = typename std::common_type_t<L, R>;

		return xellanix::math::mod_floor(xellanix::utility::cast_to<ct>(number), xellanix::utility::cast_to<ct>(divisor));
	}
}