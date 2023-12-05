#pragma once

#ifndef XELLANIX_NUMBER_H
#define XELLANIX_NUMBER_H
#endif // !XELLANIX_NUMBER_H

#ifndef XELLANIX_COMPARE_HELPER_H
#include "compare_helper.h"
#endif // !XELLANIX_COMPARE_HELPER_H

#ifndef XELLANIX_MATH_LOGICAL_H
#include "math_logical.h"
#endif // !XELLANIX_MATH_LOGICAL_H

#include <variant>
#include <ostream>

namespace xellanix::type
{
	// Index:
	// 0: char
	// 2: short
	// 4: int
	// 6: long long
	// 8: float
	struct number
	{
	private:
		std::variant<char, unsigned char,
			short, unsigned short, int, unsigned int, long long, unsigned long long,
			float, double> m_variants;

		constexpr auto is_safe_sum(long long a, long long b) const
		{
			if (((b > 0) && (a > (helper::nmax<long long>() - b))) ||
				((b < 0) && (a < (helper::nmin<long long>() - b))))
			{
				return false;
			}

			return true;
		}
		constexpr auto is_safe_subtract(long long a, long long b) const
		{
			if (((b > 0) && (a < ((std::numeric_limits<long long>::min)() + b))) || 
				((b < 0) && (a > ((std::numeric_limits<long long>::max)() + b))))
			{
				return false;
			}

			return true;
		}
		constexpr std::pair<bool, bool> is_safe_multiply(long long a, long long b) const
		{
			if (a == 0 || b == 0) return { true, false };

			constexpr auto lmin = (double)helper::nmin<long long>();
			constexpr auto lmax = (double)helper::nmax<long long>();

			const bool is_overflow_min = ((a < 0) != (b < 0)) && xellanix::math::greater(xellanix::math::abs(b), xellanix::math::abs(lmin / a));
			const bool is_overflow_max = ((a < 0) == (b < 0)) && xellanix::math::greater(xellanix::math::abs(b), xellanix::math::abs(lmax / a));

			if (is_overflow_max)
			{
				const bool is_overflow_max2 = ((a < 0) == (b < 0)) && xellanix::math::greater(xellanix::math::abs(b), xellanix::math::abs((double)helper::nmax<unsigned long long>() / a));
				
				return { false, !is_overflow_max2 };
			}

			return { !is_overflow_min, false };
		}

		template<typename UnsignedT, typename SignedT, std::enable_if_t<std::is_unsigned_v<UnsignedT> && std::is_signed_v<SignedT>, bool> = true>
		constexpr auto signdiff_add(UnsignedT lhs, SignedT rhs) const
		{
			// Needs Triage (Negative + Positive)
			const auto is_negative = rhs < 0;

			if (is_negative && xellanix::math::less(lhs, xellanix::math::abs(rhs)))
			{
				// Negative result
				auto finalRes = rhs + xellanix::utility::make_signed<UnsignedT, long long>(lhs);
				return compress_type(finalRes);
			}

			// Positive result
			auto finalRes = lhs + rhs;
			return compress_type(finalRes);
		}

		template<typename SignedT, typename UnsignedT, std::enable_if_t<std::is_signed_v<SignedT>&& std::is_unsigned_v<UnsignedT>, bool> = true>
		constexpr auto signdiff_add(SignedT lhs, UnsignedT rhs) const
		{
			return signdiff_add(rhs, lhs);
		}

		template<typename UnsignedT, typename SignedT, std::enable_if_t<std::is_unsigned_v<UnsignedT>&& std::is_signed_v<SignedT>, bool> = true>
		constexpr auto signdiff_subtract(UnsignedT lhs, SignedT rhs) const
		{
			// Needs Triage (Negative - Positive)
			if (xellanix::math::less(lhs, rhs))
			{
				// Negative result
				auto finalRes = xellanix::utility::make_signed<UnsignedT, long long>(lhs) - rhs;
				return compress_type(finalRes);
			}

			// Positive result
			auto finalRes = lhs - rhs;
			return compress_type(finalRes);
		}

		template<typename SignedT, typename UnsignedT, std::enable_if_t<std::is_signed_v<SignedT>&& std::is_unsigned_v<UnsignedT>, bool> = true>
		constexpr auto signdiff_subtract(SignedT lhs, UnsignedT rhs) const
		{
			// Needs Triage (Negative - Positive)
			if (xellanix::math::less(lhs, rhs))
			{
				// Negative result
				auto finalRes = lhs - xellanix::utility::make_signed<UnsignedT, long long>(rhs);
				return compress_type(finalRes);
			}

			// Positive result
			auto finalRes = lhs - rhs;
			return compress_type(finalRes);
		}

		template<typename UnsignedT, typename SignedT, std::enable_if_t<std::is_unsigned_v<UnsignedT>&& std::is_signed_v<SignedT>, bool> = true>
		constexpr auto signdiff_multiply(UnsignedT lhs, SignedT rhs) const
		{
			// Needs Triage (Negative + Positive)
			const auto is_negative = rhs < 0;

			if (is_negative)
			{
				// Negative result
				if (xellanix::math::greater(lhs, helper::nmax<long long>())) return number{ infinity() };

				const auto _ll = xellanix::utility::make_signed<UnsignedT, long long>(lhs);
				if (auto _is_safe = is_safe_multiply(_ll, rhs); _is_safe.first)
				{
					auto finalRes = _ll * rhs;

					return compress_type(finalRes);
				}
				else
				{
					// The result is not safe:
					// overflowing the min of long long
					return number{ infinity() };
				}
			}

			// Positive result
			const auto urhs = xellanix::utility::make_unsigned(rhs);
			const bool is_overflow = xellanix::math::greater(urhs, (double)helper::nmax<unsigned long long>() / lhs);
			if (is_overflow) return number{ infinity() };
			else
			{
				auto finalRes = lhs * urhs;
				return compress_type(finalRes);
			}
		}

		template<typename SignedT, typename UnsignedT, std::enable_if_t<std::is_signed_v<SignedT>&& std::is_unsigned_v<UnsignedT>, bool> = true>
		constexpr auto signdiff_multiply(SignedT lhs, UnsignedT rhs) const
		{
			return signdiff_multiply(rhs, lhs);
		}

		template<typename UnsignedT, typename SignedT, std::enable_if_t<std::is_unsigned_v<UnsignedT>&& std::is_signed_v<SignedT>, bool> = true>
		constexpr auto signdiff_division(UnsignedT lhs, SignedT rhs) const
		{
			// Needs Triage (Negative + Positive)
			const auto is_negative = rhs < 0;

			if (is_negative)
			{
				// Negative result
				const auto no_sign = xellanix::math::abs(rhs);

				const auto div = lhs / no_sign;

				constexpr auto ABS_LLONG_MIN = 9223372036854775808;
				if (xellanix::math::greater(div, ABS_LLONG_MIN)) return number{ negative_infinity() };
				else if (xellanix::math::equal(div, ABS_LLONG_MIN)) return number{ helper::nmin<long long>() };

				const auto div_s = xellanix::utility::cast_to<long long>(div);

				return compress_type(div_s * (-1));
			}

			// Positive result
			const auto urhs = xellanix::utility::make_unsigned(rhs);
			const auto finalRes = lhs / urhs;
			return compress_type(finalRes);
		}

		template<typename SignedT, typename UnsignedT, std::enable_if_t<std::is_signed_v<SignedT>&& std::is_unsigned_v<UnsignedT>, bool> = true>
		constexpr auto signdiff_division(SignedT lhs, UnsignedT rhs) const
		{
			// Needs Triage (Negative + Positive)
			const auto is_negative = lhs < 0;

			if (is_negative)
			{
				// Negative result
				const auto no_sign = xellanix::math::abs(lhs);

				const auto div = no_sign / rhs;

				constexpr auto ABS_LLONG_MIN = 9223372036854775808;
				if (xellanix::math::greater(div, ABS_LLONG_MIN)) return number{ negative_infinity() };
				else if (xellanix::math::equal(div, ABS_LLONG_MIN)) return number{ helper::nmin<long long>() };

				const auto div_s = xellanix::utility::cast_to<long long>(div);

				return compress_type(div_s * (-1));
			}

			// Positive result
			const auto ulhs = xellanix::utility::make_unsigned(lhs);
			const auto finalRes = ulhs / rhs;
			return compress_type(finalRes);
		}

		template<typename UnsignedT, typename SignedT, std::enable_if_t<std::is_unsigned_v<UnsignedT>&& std::is_signed_v<SignedT>, bool> = true>
		constexpr auto signdiff_division2(UnsignedT lhs, SignedT rhs) const
		{
			// Needs Triage (Negative + Positive)
			const auto is_negative = rhs < 0;

			if (is_negative)
			{
				// Negative result
				const auto no_sign = xellanix::math::abs(rhs);
				
				if (xellanix::math::is_divres_int(lhs, no_sign))
				{
					const auto div = lhs / no_sign;

					constexpr auto ABS_LLONG_MIN = 9223372036854775808;
					if (xellanix::math::greater(div, ABS_LLONG_MIN)) return number{ negative_infinity() };
					else if (xellanix::math::equal(div, ABS_LLONG_MIN)) return number{ helper::nmin<long long>() };

					const auto div_s = xellanix::utility::cast_to<long long>(div);

					return compress_type(div_s * (-1));
				}
				else
				{
					const auto div = (double)lhs / no_sign;

					constexpr auto ABS_LLONG_MIN = 9223372036854775808.0;
					if (xellanix::math::greater(div, ABS_LLONG_MIN)) return number{ negative_infinity() };
					else if (xellanix::math::equal(div, ABS_LLONG_MIN)) return number{ helper::nmin<long long>() };

					return number{ div * (-1) };
				}
			}

			// Positive result
			const auto urhs = xellanix::utility::make_unsigned(rhs);
			if (xellanix::math::is_divres_int(lhs, urhs))
			{
				const auto finalRes = lhs / urhs;
				return compress_type(finalRes);
			}
			else
			{
				const auto finalRes = (double)lhs / rhs;
				return number{ finalRes };
			}
		}

		template<typename SignedT, typename UnsignedT, std::enable_if_t<std::is_signed_v<SignedT>&& std::is_unsigned_v<UnsignedT>, bool> = true>
		constexpr auto signdiff_division2(SignedT lhs, UnsignedT rhs) const
		{
			// Needs Triage (Negative + Positive)
			const auto is_negative = lhs < 0;

			if (is_negative)
			{
				// Negative result
				const auto no_sign = xellanix::math::abs(lhs);

				if (const auto is_integer = (no_sign % rhs) == 0; is_integer)
				{
					const auto div = no_sign / rhs;

					constexpr auto ABS_LLONG_MIN = 9223372036854775808;
					if (xellanix::math::greater(div, ABS_LLONG_MIN)) return number{ negative_infinity() };
					else if (xellanix::math::equal(div, ABS_LLONG_MIN)) return number{ helper::nmin<long long>() };

					const auto div_s = xellanix::utility::cast_to<long long>(div);

					return compress_type(div_s * (-1));
				}
				else
				{
					const auto div = no_sign / (double)rhs;

					constexpr auto ABS_LLONG_MIN = 9223372036854775808.0;
					if (xellanix::math::greater(div, ABS_LLONG_MIN)) return number{ negative_infinity() };
					else if (xellanix::math::equal(div, ABS_LLONG_MIN)) return number{ helper::nmin<long long>() };

					return number{ div * (-1) };
				}
			}

			// Positive result
			const auto ulhs = xellanix::utility::make_unsigned(lhs);
			if (const auto is_integer = (ulhs % rhs) == 0; is_integer)
			{
				const auto finalRes = ulhs / rhs;
				return compress_type(finalRes);
			}
			else
			{
				const auto finalRes = (double)lhs / rhs;
				return number{ finalRes };
			}
		}

		// Negative Type
		template<typename T, typename V>
		constexpr helper::enable_if_signed<T> is_in_limit(V value) const
		{
			return xellanix::math::less_eq((std::numeric_limits<T>::min)(), value);
		}

		// Positive Type
		template<typename T, typename V>
		constexpr helper::enable_if_unsigned<T> is_in_limit(V value) const
		{
			return xellanix::math::less_eq(value, (std::numeric_limits<T>::max)());
		}

		template<typename T>
		constexpr std::enable_if_t<std::is_signed_v<T>, number> compress_type(T value) const
		{
			const auto is_result_negative = value < 0;
			if (is_result_negative)
			{
				// Set the 'number' type to the smallest possible `signed` type
				if (is_in_limit<char>(value))
				{
					return xellanix::utility::cast_to<char>(value);
				}
				else if (is_in_limit<short>(value))
				{
					return xellanix::utility::cast_to<short>(value);
				}
				else if (is_in_limit<int>(value))
				{
					return xellanix::utility::cast_to<int>(value);
				}
				else if (is_in_limit<long long>(value))
				{
					return xellanix::utility::cast_to<long long>(value);
				}
			}
			else
			{
				// Set the 'number' type to the smallest possible `unsigned` type
				if (is_in_limit<unsigned char>(value))
				{
					return xellanix::utility::cast_to<unsigned char>(value);
				}
				else if (is_in_limit<unsigned short>(value))
				{
					return xellanix::utility::cast_to<unsigned short>(value);
				}
				else if (is_in_limit<unsigned int>(value))
				{
					return xellanix::utility::cast_to<unsigned int>(value);
				}
				else if (is_in_limit<unsigned long long>(value))
				{
					return xellanix::utility::cast_to<unsigned long long>(value);
				}
			}

			return number{ std::numeric_limits<double>::quiet_NaN() };
		}

		template<typename T>
		constexpr std::enable_if_t<std::is_unsigned_v<T>, number> compress_type(T value) const
		{
			// Set the 'number' type to the smallest possible `unsigned` type
			if (is_in_limit<unsigned char>(value))
			{
				return xellanix::utility::cast_to<unsigned char>(value);
			}
			else if (is_in_limit<unsigned short>(value))
			{
				return xellanix::utility::cast_to<unsigned short>(value);
			}
			else if (is_in_limit<unsigned int>(value))
			{
				return xellanix::utility::cast_to<unsigned int>(value);
			}
			else if (is_in_limit<unsigned long long>(value))
			{
				return xellanix::utility::cast_to<unsigned long long>(value);
			}

			return number{ std::numeric_limits<double>::quiet_NaN() };
		}

		constexpr bool is_integral(short idx) const
		{
			return idx < 8;
		}
		constexpr bool is_floating_point(short idx) const
		{
			return idx >= 8;
		}
		constexpr bool is_unsigned(short idx) const
		{
			return idx < 8 ? (idx % 2) : false;
		}
		constexpr bool is_signed(short idx) const
		{
			return !is_unsigned(idx);
		}

	public:
		// Constructors
		constexpr number()
		{
			m_variants = decltype(m_variants){ char(0) };
		}

		template<typename T, helper::enable_if_arithm<T> = true>
		constexpr number(T value)
		{
			using variant_t = decltype(m_variants);

			if constexpr (std::is_same_v<T, bool>)
			{
				const variant_t _{ xellanix::utility::cast_to<unsigned char>(value) };
				m_variants = std::move(_);
			}
			#ifdef __cpp_char8_t
			// To char8_t
			else if constexpr (std::is_same_v<T, char8_t>)
			{
				const variant_t _{ xellanix::utility::cast_to<unsigned char>(value) };
				m_variants = std::move(_);
			}
			#endif // __cpp_char8_t
			else if constexpr ((std::is_same_v<T, wchar_t> && xellanix::type::helper::is_same_limit<wchar_t, unsigned short>()) ||
							   std::is_same_v<T, char16_t>)
			{
				const variant_t _{ xellanix::utility::cast_to<unsigned short>(value) };
				m_variants = std::move(_);
			}
			else if constexpr ((std::is_same_v<T, wchar_t> && xellanix::type::helper::is_same_limit<wchar_t, unsigned int>()) ||
							   std::is_same_v<T, unsigned long> || std::is_same_v<T, char32_t>)
			{
				const variant_t _{ xellanix::utility::cast_to<unsigned int>(value) };
				m_variants = std::move(_);
			}
			else if constexpr (std::is_same_v<T, long>)
			{
				const variant_t _{ xellanix::utility::cast_to<int>(value) };
				m_variants = std::move(_);
			}
			else
			{
				const variant_t _{ value };
				m_variants = std::move(_);
			}
		}

		// Member Functions
		constexpr unsigned short index() const noexcept
		{
			return xellanix::utility::cast_to<unsigned short>(m_variants.index());
		}

		constexpr bool is_integral() const noexcept
		{
			auto idx = m_variants.index();
			return idx < 8;
		}
		constexpr bool is_floating_point() const noexcept
		{
			auto idx = m_variants.index();
			return idx >= 8;
		}

		constexpr bool is_unsigned() const noexcept
		{
			auto idx = m_variants.index();
			return idx < 8 ? (idx % 2) : false;
		}
		constexpr bool is_signed() const noexcept
		{
			return !is_unsigned();
		}

		constexpr bool is_negative() const
		{
			const auto extractedValue = visit([](auto&& arg) -> bool
			{
				decltype(arg) comparer = 0;
				return xellanix::math::less(arg, comparer);
			});

			return extractedValue;
		}
		constexpr bool is_positive() const
		{
			return !is_negative();
		}

		constexpr bool is_infinity() const
		{
			const auto is_inf = visit([](auto&& arg) -> bool
			{
				if constexpr (std::is_floating_point_v<decltype(arg)>)
				{
					return xellanix::math::is_inf(arg);
				}
				else return false;
			});

			return is_inf;
		}
		constexpr bool is_positive_infinity() const
		{
			const auto is_inf = visit([](auto&& arg) -> bool
			{
				using ArgT = decltype(arg);
				if constexpr (std::is_floating_point_v<ArgT>)
				{
					if (xellanix::math::is_inf(arg)) return arg >= xellanix::utility::cast_to<ArgT>(0.0);
				}

				return false;
			});

			return is_inf;
		}
		constexpr bool is_negative_infinity() const
		{
			const auto is_inf = visit([](auto&& arg) -> bool
			{
				using ArgT = decltype(arg);
				if constexpr (std::is_floating_point_v<ArgT>)
				{
					if (xellanix::math::is_inf(arg)) return arg < xellanix::utility::cast_to<ArgT>(0.0);
				}

				return false;
			});

			return is_inf;
		}

		constexpr static double infinity() noexcept
		{
			return std::numeric_limits<double>::infinity();
		}
		constexpr static double negative_infinity() noexcept
		{
			return -infinity();
		}

		constexpr static float infinity_f() noexcept
		{
			return std::numeric_limits<float>::infinity();
		}
		constexpr static float negative_infinity_f() noexcept
		{
			return -infinity_f();
		}

		// 0 based NaN
		constexpr static double quiet_nan() noexcept
		{
			return std::numeric_limits<double>::quiet_NaN();
		}
		// 1 based Nan
		constexpr static double signaling_nan() noexcept
		{
			return std::numeric_limits<double>::signaling_NaN();
		}

		// 0 based NaN
		constexpr static float quiet_nan_f() noexcept
		{
			return std::numeric_limits<float>::quiet_NaN();
		}
		// 1 based Nan
		constexpr static float signaling_nan_f() noexcept
		{
			return std::numeric_limits<float>::signaling_NaN();
		}

		// Visit
		template<typename Fn>
		constexpr decltype(auto) visit(Fn&& callable) const
		{
			return std::visit(std::forward<Fn>(callable), m_variants);
		}

		//	Cast Functions
		template<typename T>
		constexpr helper::enable_if_arithm_t<T> cast_to() const
		{
			const auto extractedValue = visit([](auto&& arg)
			{
				return xellanix::utility::cast_to<T>(arg);
			});

			return extractedValue;
		}

		// To bool
		constexpr auto b() const
		{
			return cast_to<bool>();
		}
		// To char
		constexpr auto c() const
		{
			return cast_to<char>();
		}
		// To signed char
		constexpr auto sc() const
		{
			return cast_to<signed char>();
		}
		// To unsigned char
		constexpr auto uc() const
		{
			return cast_to<unsigned char>();
		}
		// To wchar_t
		constexpr auto wc() const
		{
			return cast_to<wchar_t>();
		}
		#ifdef __cpp_char8_t
		// To char8_t
		constexpr auto c8() const
		{
			return cast_to<char8_t>();
		}
		#endif // __cpp_char8_t
		// To char16_t
		constexpr auto c16() const
		{
			return cast_to<char16_t>();
		}
		// To char32_t
		constexpr auto c32() const
		{
			return cast_to<char32_t>();
		}
		// To short
		constexpr auto s() const
		{
			return cast_to<short>();
		}
		// To unsigned short
		constexpr auto us() const
		{
			return cast_to<unsigned short>();
		}
		// To int
		constexpr auto i() const
		{
			return cast_to<int>();
		}
		// To unsigned int
		constexpr auto ui() const
		{
			return cast_to<unsigned int>();
		}
		// To long
		constexpr auto l() const
		{
			return cast_to<long>();
		}
		// To unsigned long
		constexpr auto ul() const
		{
			return cast_to<unsigned long>();
		}
		// To long long
		constexpr auto ll() const
		{
			return cast_to<long long>();
		}
		// To unsigned long long
		constexpr auto ull() const
		{
			return cast_to<unsigned long long>();
		}
		// To float
		constexpr auto f() const
		{
			return cast_to<float>();
		}
		// To double
		constexpr auto d() const
		{
			return cast_to<double>();
		}
		// To long double
		constexpr auto ld() const
		{
			return cast_to<long double>();
		}

		// Add operators
		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		constexpr auto operator+(SignedT value) const
		{
			constexpr auto is_right_floating = std::is_floating_point_v<SignedT>;

			if constexpr (!is_right_floating)
			{
				if (xellanix::math::equal(value, (SignedT)0))
				{
					return *this;
				}
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();

				if constexpr (is_right_floating)
				{
					auto finalRes = res + value;
					return number{ finalRes };
				}
				else
				{
					return signdiff_add(res, value);
				}
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res + value;
					return number{ finalRes };
				}
				else
				{
					if constexpr (is_right_floating)
					{
						auto finalRes = ll() + value;
						return number{ finalRes };
					}
					else
					{
						if (auto _ll = ll(); is_safe_sum(_ll, value))
						{
							auto finalRes = _ll + value;

							return compress_type(finalRes);
						}
						else
						{
							// If the left is negative, don't cast it to unsigned
							if (_ll < 0) return number{ negative_infinity() };
							else
							{
								// The left is positive, cast it to unsigned (safe)
								return signdiff_add(ull(), value);
							}
						}
					}
				}
			}
		}

		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		constexpr auto operator+(UnsignedT valueTemp) const
		{
			using _NonBoolT = typename std::conditional_t<std::is_same_v<std::remove_cv_t<UnsignedT>, bool>, unsigned char, UnsignedT>;
			_NonBoolT value = valueTemp;

			if (xellanix::math::equal(value, (UnsignedT)0))
			{
				return *this;
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();
				auto finalRes = res + value;

				return compress_type(finalRes);
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res + value;
					return number{ finalRes };
				}
				else
				{
					auto res = ll();

					return signdiff_add(res, value);
				}
			}
		}

		template<typename T, helper::enable_if_any_signed<T> = true>
		friend constexpr auto operator+(T lhs, xellanix::type::number const& rhs)
		{
			return rhs + lhs;
		}

		template<typename T, helper::enable_if_same<T, xellanix::type::number> = true>
		constexpr auto operator+(T const& value) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();
				return res + value;
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();
					return res + value;
				}
				else
				{
					auto res = ll();
					return res + value;
				}
			}
		}

		// Subtract operators
		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		constexpr auto operator-(SignedT value) const
		{
			constexpr auto is_right_floating = std::is_floating_point<SignedT>;

			if constexpr (!is_right_floating)
			{
				if (xellanix::math::equal(value, (SignedT)0))
				{
					return *this;
				}
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();

				if constexpr (is_right_floating)
				{
					auto finalRes = res - value;
					return number{ finalRes };
				}
				else
				{
					return signdiff_subtract(res, value);
				}
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res - value;
					return number{ finalRes };
				}
				else
				{
					if constexpr (is_right_floating)
					{
						return number{ ll() - value };
					}
					else
					{
						if (auto _ll = ll(); is_safe_subtract(_ll, value))
						{
							auto finalRes = _ll - value;

							return compress_type(finalRes);
						}
						else
						{
							// If the left is negative, don't cast it to unsigned
							if (_ll < 0) return number{ negative_infinity() };
							else
							{
								// The left is positive, cast it to unsigned (safe)
								return signdiff_subtract(ull(), value);
							}
						}
					}
				}
			}
		}

		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		constexpr auto operator-(UnsignedT value) const
		{
			if (xellanix::math::equal(value, (UnsignedT)0))
			{
				return *this;
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();

				// Needs Triage (Maybe Negative FinalRes)
				if (xellanix::math::less(res, value))
				{
					// Negative result
					auto finalRes = -xellanix::utility::make_signed(value - res);
					return compress_type(finalRes);
				}

				// Positive result
				auto finalRes = res - value;
				return compress_type(finalRes);
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res - value;
					return number{ finalRes };
				}
				else
				{
					auto res = ll();

					return signdiff_subtract(res, value);
				}
			}
		}

		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		friend constexpr auto operator-(SignedT lhs_raw, xellanix::type::number const& rhs)
		{
			constexpr auto is_left_floating = std::is_floating_point_v<SignedT>;
			using highest_t = std::conditional_t<is_left_floating, double, long long>;

			highest_t lhs = xellanix::utility::cast_to<highest_t>(lhs_raw);
			const auto ridx = rhs.index();

			if (rhs.is_unsigned(ridx))
			{
				auto res = rhs.ull();

				if constexpr (is_left_floating)
				{
					auto finalRes = lhs - res;
					return xellanix::type::number{ finalRes };
				}
				else
				{
					return rhs.signdiff_subtract(lhs, res);
				}
			}
			else
			{
				if (rhs.is_floating_point(ridx))
				{
					auto res = rhs.d();

					auto finalRes = lhs - res;
					return xellanix::type::number{ finalRes };
				}
				else
				{
					if constexpr (is_left_floating)
					{
						return xellanix::type::number{ lhs - rhs.ll() };
					}
					else
					{
						if (auto _ll = rhs.ll(); rhs.is_safe_subtract(lhs, _ll))
						{
							auto finalRes = lhs - _ll;

							return rhs.compress_type(finalRes);
						}
						else
						{
							// If the left is negative, don't cast it to unsigned
							if (lhs < 0) return xellanix::type::number{ number::negative_infinity() };
							else
							{
								// The left is positive, cast it to unsigned (safe)
								return rhs.signdiff_subtract(xellanix::utility::make_unsigned(lhs), _ll);
							}
						}
					}
				}
			}
		}

		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		friend constexpr auto operator-(UnsignedT lhs_raw, xellanix::type::number const& rhs)
		{
			auto lhs = xellanix::utility::cast_to<unsigned long long>(lhs_raw);

			const auto ridx = rhs.index();

			if (rhs.is_unsigned(ridx))
			{
				auto res = rhs.ull();

				// Needs Triage (Maybe Negative FinalRes)
				if (xellanix::math::less(lhs, res))
				{
					// Negative result
					auto finalRes = -xellanix::utility::make_signed(res - lhs);
					return rhs.compress_type(finalRes);
				}

				// Positive result
				auto finalRes = lhs - res;
				return rhs.compress_type(finalRes);
			}
			else
			{
				if (rhs.is_floating_point(ridx))
				{
					auto res = rhs.d();

					auto finalRes = lhs - res;
					return xellanix::type::number{ finalRes };
				}
				else
				{
					auto res = rhs.ll();

					return rhs.signdiff_subtract(lhs, res);
				}
			}
		}

		template<typename T, helper::enable_if_same<T, xellanix::type::number> = true>
		constexpr auto operator-(T const& value) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();
				return res - value;
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();
					return res - value;
				}
				else
				{
					auto res = ll();
					return res - value;
				}
			}
		}

		// Multiply operators
		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		constexpr auto operator*(SignedT value) const
		{
			constexpr auto is_right_floating = std::is_floating_point_v<SignedT>;

			if constexpr (!is_right_floating)
			{
				if (xellanix::math::equal(value, (SignedT)1))
				{
					return *this;
				}
				else if (xellanix::math::equal(value, (SignedT)0))
				{
					return number{};
				}
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();

				if constexpr (is_right_floating)
				{
					auto finalRes = res * value;
					return number{ finalRes };
				}
				else
				{
					return signdiff_multiply(res, value);
				}
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res * value;
					return number{ finalRes };
				}
				else
				{
					if constexpr (is_right_floating)
					{
						auto finalRes = ll() * value;
						return number{ finalRes };
					}
					else
					{
						auto _ll = ll();
						if (auto _is_safe = is_safe_multiply(_ll, value); _is_safe.first)
						{
							auto finalRes = _ll * value;

							return compress_type(finalRes);
						}
						else
						{
							if (_is_safe.second)
							{
								// The result is positive, and not overflowing
								// the max of unsigned long long
								auto finalRes = xellanix::math::abs(_ll) * xellanix::math::abs(value);

								return compress_type(finalRes);
							}
							else
							{
								// The result is not safe:
								// maybe overflowing the min of long long
								// maybe overflowing the max of unsigned long long
								return number{ infinity() };
							}
						}
					}
				}
			}
		}

		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		constexpr auto operator*(UnsignedT valueTemp) const
		{
			using _NonBoolT = typename std::conditional_t<helper::is_same_r_v<UnsignedT, bool>, unsigned char, UnsignedT>;
			_NonBoolT value = valueTemp;

			if (xellanix::math::equal(value, (UnsignedT)1))
			{
				return *this;
			}
			else if (xellanix::math::equal(value, (UnsignedT)0))
			{
				return number{};
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();
				const bool is_overflow = xellanix::math::greater(value, (double)helper::nmax<unsigned long long>() / res);
				if (is_overflow) return number{ infinity() };
				else
				{
					auto finalRes = res * value;
					return compress_type(finalRes);
				}
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res * value;
					return number{ finalRes };
				}
				else
				{
					auto res = ll();

					return signdiff_multiply(res, value);
				}
			}
		}

		template<typename T, helper::enable_if_any_signed<T> = true>
		friend constexpr auto operator*(T lhs, xellanix::type::number const& rhs)
		{
			return rhs * lhs;
		}

		template<typename T, helper::enable_if_same<T, xellanix::type::number> = true>
		constexpr auto operator*(T const& value) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();
				return res * value;
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();
					return res * value;
				}
				else
				{
					auto res = ll();
					return res * value;
				}
			}
		}

		// Division operators
		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		constexpr auto operator/(SignedT value) const
		{
			constexpr auto is_right_floating = std::is_floating_point_v<SignedT>;

			if constexpr (!is_right_floating)
			{
				if (xellanix::math::equal(value, (SignedT)1))
				{
					return *this;
				}
				else if (xellanix::math::equal(value, (SignedT)0))
				{
					return number{ infinity() };
				}
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();

				if constexpr (is_right_floating)
				{
					auto finalRes = res / value;
					return number{ finalRes };
				}
				else
				{
					return signdiff_division(res, value);
				}
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res / value;
					return number{ finalRes };
				}
				else
				{
					const auto _ll = ll();

					if constexpr (is_right_floating)
					{
						auto finalRes = _ll / value;
						return number{ finalRes };
					}
					else
					{
						auto finalRes = _ll / value;

						return compress_type(finalRes);
					}
				}
			}
		}

		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		constexpr auto operator/(UnsignedT valueTemp) const
		{
			using _NonBoolT = typename std::conditional_t<helper::is_same_r_v<UnsignedT, bool>, unsigned char, UnsignedT>;
			_NonBoolT value = valueTemp;

			if (xellanix::math::equal(value, (UnsignedT)1))
			{
				return *this;
			}
			else if (xellanix::math::equal(value, (UnsignedT)0))
			{
				return number{ infinity() };
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto finalRes = ull() / value;

				return compress_type(finalRes);
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res / value;
					return number{ finalRes };
				}
				else
				{
					auto res = ll();

					return signdiff_division(res, value);
				}
			}
		}

		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		friend constexpr auto operator/(SignedT lhs_raw, xellanix::type::number const& rhs)
		{
			constexpr auto is_left_floating = std::is_floating_point_v<SignedT>;
			using highest_t = std::conditional_t<is_left_floating, double, long long>;

			highest_t lhs = xellanix::utility::cast_to<highest_t>(lhs_raw);

			if constexpr (!is_left_floating)
			{
				if (xellanix::math::equal(lhs, (highest_t)0))
				{
					return number{};
				}
			}

			const auto ridx = rhs.index();

			if (rhs.is_unsigned(ridx))
			{
				auto res = rhs.ull();

				if constexpr (is_left_floating)
				{
					auto finalRes = lhs / res;
					return number{ finalRes };
				}
				else
				{
					return rhs.signdiff_division(lhs, res);
				}
			}
			else
			{
				if (rhs.is_floating_point(ridx))
				{
					auto res = rhs.d();

					auto finalRes = lhs / res;
					return number{ finalRes };
				}
				else
				{
					if constexpr (is_left_floating)
					{
						return number{ lhs / rhs.ll() };
					}
					else
					{
						auto finalRes = lhs / rhs.ll();

						return rhs.compress_type(finalRes);
					}
				}
			}
		}

		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		friend constexpr auto operator/(UnsignedT lhs_raw, xellanix::type::number const& rhs)
		{
			auto lhs = xellanix::utility::cast_to<unsigned long long>(lhs_raw);

			if (xellanix::math::equal(lhs, 0ull))
			{
				return number{};
			}

			const auto ridx = rhs.index();

			if (rhs.is_unsigned(ridx))
			{
				auto finalRes = lhs / rhs.ull();

				return rhs.compress_type(finalRes);
			}
			else
			{
				if (rhs.is_floating_point(ridx))
				{
					auto res = rhs.d();

					auto finalRes = lhs / res;
					return number{ finalRes };
				}
				else
				{
					auto res = rhs.ll();

					return rhs.signdiff_division(lhs, res);
				}
			}
		}

		template<typename T, helper::enable_if_same<T, xellanix::type::number> = true>
		constexpr auto operator/(T const& value) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();
				return res / value;
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();
					return res / value;
				}
				else
				{
					auto res = ll();
					return res / value;
				}
			}
		}

		// Modulo operators
		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		constexpr auto operator%(SignedT right) const
		{
			return visit([right, this](auto&& left)
			{
				// is lefts signed or unsigned?
				// if signed, it's safe
				// but if unsigned? it's not

				if constexpr (std::is_floating_point_v<SignedT>)
				{
					return number{ xellanix::math::mod(left, right) };
				}
				else
				{
					if (is_floating_point())
					{
						return number{ xellanix::math::mod(left, right) };
					}
					else if (is_unsigned())
					{
						if (xellanix::math::is_negative(right))
						{
							if (xellanix::math::greater(left, helper::nmax<long long>()))
							{
								const auto absv = xellanix::math::abs(right);

								const auto ab = left / absv;
								const auto ab2 = ab * absv + absv;

								return compress_type(xellanix::utility::cast_to<long long>(left - ab2));
							}
							else
							{
								const auto ll = xellanix::utility::cast_to<long long>(left);

								return compress_type(xellanix::math::mod(ll, right));
							}
						}
					}

					return compress_type(xellanix::math::mod(left, right));
				}
			});
		}

		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		constexpr auto operator%(UnsignedT valueTemp) const
		{
			using _NonBoolT = typename std::conditional_t<helper::is_same_r_v<UnsignedT, bool>, unsigned char, UnsignedT>;
			_NonBoolT right = valueTemp;

			return visit([right, this](auto&& left)
			{
				if (is_floating_point())
				{
					return number{ xellanix::math::mod(left, right) };
				}
				else if (is_negative())
				{
					if (xellanix::math::greater(right, helper::nmax<long long>()))
					{
						const auto absv = xellanix::math::abs(left);

						const auto ab = absv / right;
						const auto ab2 = ab * absv + absv;

						return compress_type(xellanix::utility::cast_to<long long>(left + ab2));
					}
					else
					{
						const auto ll = xellanix::utility::cast_to<long long>(right);

						return compress_type(xellanix::math::mod(left, ll));
					}
				}

				return compress_type(xellanix::math::mod(left, right));
			});
		}

		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		friend constexpr auto operator%(SignedT left, xellanix::type::number const& rhs)
		{
			return rhs.visit([left, &rhs](auto&& right)
			{
				if constexpr (std::is_floating_point_v<SignedT>)
				{
					return number{ xellanix::math::mod(left, right) };
				}
				else
				{
					if (rhs.is_floating_point())
					{
						return number{ xellanix::math::mod(left, right) };
					}
					else if (rhs.is_unsigned())
					{
						if (xellanix::math::is_negative(left))
						{
							if (xellanix::math::greater(right, helper::nmax<long long>()))
							{
								const auto absv = xellanix::math::abs(left);

								const auto ab = absv / right;
								const auto ab2 = ab * absv + absv;

								return rhs.compress_type(xellanix::utility::cast_to<long long>(left + ab2));
							}
							else
							{
								const auto ll = xellanix::utility::cast_to<long long>(right);

								return rhs.compress_type(xellanix::math::mod(left, ll));
							}
						}
					}

					return rhs.compress_type(xellanix::math::mod(left, right));
				}
			});
		}

		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		friend constexpr auto operator%(UnsignedT valueTemp, xellanix::type::number const& rhs)
		{
			using _NonBoolT = typename std::conditional_t<helper::is_same_r_v<UnsignedT, bool>, unsigned char, UnsignedT>;
			_NonBoolT left = valueTemp;

			return rhs.visit([left, &rhs](auto&& right)
			{
				if (rhs.is_floating_point())
				{
					return number{ xellanix::math::mod(left, right) };
				}
				else if (rhs.is_negative())
				{
					if (xellanix::math::greater(left, helper::nmax<long long>()))
					{
						const auto absv = xellanix::math::abs(right);

						const auto ab = left / absv;
						const auto ab2 = ab * absv + absv;

						return rhs.compress_type(xellanix::utility::cast_to<long long>(left - ab2));
					}
					else
					{
						const auto ll = xellanix::utility::cast_to<long long>(left);

						return rhs.compress_type(xellanix::math::mod(ll, right));
					}
				}

				return rhs.compress_type(xellanix::math::mod(left, right));
			});
		}

		template<typename T, helper::enable_if_same<T, xellanix::type::number> = true>
		constexpr auto operator%(T const& value) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();
				return res % value;
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();
					return res % value;
				}
				else
				{
					auto res = ll();
					return res % value;
				}
			}
		}

		// Arithmetic assignment operators
		// operator= use default

		template<typename T, helper::enable_if_any_numerics<T, xellanix::type::number> = true>
		constexpr decltype(auto) operator+=(T const& rhs)
		{
			m_variants = (operator+(rhs)).m_variants;
			return *this;
		}

		template<typename T, helper::enable_if_any_numerics<T, xellanix::type::number> = true>
		constexpr decltype(auto) operator-=(T const& rhs)
		{
			m_variants = (operator-(rhs)).m_variants;
			return *this;
		}

		template<typename T, helper::enable_if_any_numerics<T, xellanix::type::number> = true>
		constexpr decltype(auto) operator*=(T const& rhs)
		{
			m_variants = (operator*(rhs)).m_variants;
			return *this;
		}

		template<typename T, helper::enable_if_any_numerics<T, xellanix::type::number> = true>
		constexpr decltype(auto) operator/=(T const& rhs)
		{
			m_variants = (operator/(rhs)).m_variants;
			return *this;
		}

		template<typename T, helper::enable_if_any_numerics<T, xellanix::type::number> = true>
		constexpr decltype(auto) operator%=(T const& rhs)
		{
			m_variants = (operator%(rhs)).m_variants;
			return *this;
		}

		// Prefix increment operators
		constexpr decltype(auto) operator++()
		{
			const auto insertVal = [&](auto value)
			{
				using variant_t = decltype(m_variants);
				using T = decltype(value);

				if constexpr (std::is_same_v<T, bool>)
				{
					m_variants = xellanix::utility::cast_to<unsigned char>(value);
				}
				#ifdef __cpp_char8_t
				// To char8_t
				else if constexpr (std::is_same_v<T, char8_t>)
				{
					m_variants = xellanix::utility::cast_to<unsigned char>(value);
				}
				#endif // __cpp_char8_t
				else if constexpr ((std::is_same_v<T, wchar_t> && xellanix::type::helper::is_same_limit<wchar_t, unsigned short>()) ||
								   std::is_same_v<T, char16_t>)
				{
					m_variants = xellanix::utility::cast_to<unsigned short>(value);
				}
				else if constexpr ((std::is_same_v<T, wchar_t> && xellanix::type::helper::is_same_limit<wchar_t, unsigned int>()) ||
								   std::is_same_v<T, unsigned long> || std::is_same_v<T, char32_t>)
				{
					m_variants = xellanix::utility::cast_to<unsigned int>(value);
				}
				else if constexpr (std::is_same_v<T, long>)
				{
					m_variants = xellanix::utility::cast_to<int>(value);
				}
				else
				{
					m_variants = value;
				}
			};

			const auto compressT = [&](auto value)
			{
				const auto is_result_negative = value < 0;
				if (is_result_negative)
				{
					// Set the 'number' type to the smallest possible `signed` type
					if (is_in_limit<char>(value))
					{
						insertVal(xellanix::utility::cast_to<char>(value));
					}
					else if (is_in_limit<short>(value))
					{
						insertVal(xellanix::utility::cast_to<short>(value));
					}
					else if (is_in_limit<int>(value))
					{
						insertVal(xellanix::utility::cast_to<int>(value));
					}
					else if (is_in_limit<long long>(value))
					{
						insertVal(xellanix::utility::cast_to<long long>(value));
					}
				}
				else
				{
					// Set the 'number' type to the smallest possible `unsigned` type
					if (is_in_limit<unsigned char>(value))
					{
						insertVal(xellanix::utility::cast_to<unsigned char>(value));
					}
					else if (is_in_limit<unsigned short>(value))
					{
						insertVal(xellanix::utility::cast_to<unsigned short>(value));
					}
					else if (is_in_limit<unsigned int>(value))
					{
						insertVal(xellanix::utility::cast_to<unsigned int>(value));
					}
					else if (is_in_limit<unsigned long long>(value))
					{
						insertVal(xellanix::utility::cast_to<unsigned long long>(value));
					}
				}

				insertVal(number::quiet_nan());
			};

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto finalRes = ull() + 1U;

				compressT(finalRes);
				return *this;
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto finalRes = d() + 1.0;
					
					insertVal(finalRes);
					return *this;
				}
				else
				{
					auto res = ll();
					const auto is_negative = res < 0;

					if (is_negative && xellanix::math::less(1ULL, xellanix::math::abs(res)))
					{
						// Negative result
						auto finalRes = res + 1;

						compressT(finalRes);
						return *this;
					}

					// Positive result
					auto finalRes = 1ULL + res;
					
					compressT(finalRes);
					return *this;
				}
			}
		}

		// Postfix increment operators

		// Prefix decrement operators

		// Postfix decrement operators

		// Equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		constexpr bool operator==(T const rhs) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				// Left unsigned
				auto res = ull();

				return xellanix::math::equal(res, rhs);
			}
			else
			{
				// Left signed
				if (is_floating_point(lidx))
				{
					// Left floating point
					auto res = d();
					return xellanix::math::equal(res, rhs);
				}
				else
				{
					// Left integral
					auto res = ll();
					return xellanix::math::equal(res, rhs);
				}
			}
		}

		// Equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		friend constexpr bool operator==(T const lhs, xellanix::type::number const& rhs)
		{
			return rhs == lhs;
		}

		// Equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_same<T, xellanix::type::number> = true>
		constexpr bool operator==(T const& rhs) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				// Left unsigned
				auto res = ull();

				return rhs == res;
			}
			else
			{
				// Left signed
				if (is_floating_point(lidx))
				{
					// Left floating point
					auto res = d();
					return rhs == res;
				}
				else
				{
					// Left integral
					auto res = ll();
					return rhs == res;
				}
			}
		}

		// Not equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_any_numerics<T, xellanix::type::number> = true>
		constexpr bool operator!=(T const& rhs) const
		{
			return !operator==(rhs);
		}

		// Not equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		friend constexpr bool operator!=(T const lhs, xellanix::type::number const& rhs)
		{
			return rhs != lhs;
		}

		// Less than operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		constexpr bool operator<(T const rhs) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				// Left unsigned
				auto res = ull();

				return xellanix::math::less(res, rhs);
			}
			else
			{
				// Left signed
				if (is_floating_point(lidx))
				{
					// Left floating point
					auto res = d();
					return xellanix::math::less(res, rhs);
				}
				else
				{
					// Left integral
					auto res = ll();
					return xellanix::math::less(res, rhs);
				}
			}
		}

		// Less than operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		friend constexpr bool operator<(T const lhs, xellanix::type::number const& rhs)
		{
			return rhs > lhs;
		}

		// Less than operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_same<T, xellanix::type::number> = true>
		constexpr bool operator<(T const& rhs) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				// Left unsigned
				auto res = ull();

				return res < rhs;
			}
			else
			{
				// Left signed
				if (is_floating_point(lidx))
				{
					// Left floating point
					auto res = d();
					return res < rhs;
				}
				else
				{
					// Left integral
					auto res = ll();
					return res < rhs;
				}
			}
		}

		// Less than or equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		constexpr bool operator<=(T const rhs) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				// Left unsigned
				auto res = ull();

				return xellanix::math::less_eq(res, rhs);
			}
			else
			{
				// Left signed
				if (is_floating_point(lidx))
				{
					// Left floating point
					auto res = d();
					return xellanix::math::less_eq(res, rhs);
				}
				else
				{
					// Left integral
					auto res = ll();
					return xellanix::math::less_eq(res, rhs);
				}
			}
		}

		// Less than or equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		friend constexpr bool operator<=(T const lhs, xellanix::type::number const& rhs)
		{
			return rhs >= lhs;
		}

		// Less than or equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_same<T, xellanix::type::number> = true>
		constexpr bool operator<=(T const& rhs) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				// Left unsigned
				auto res = ull();

				return res <= rhs;
			}
			else
			{
				// Left signed
				if (is_floating_point(lidx))
				{
					// Left floating point
					auto res = d();
					return res <= rhs;
				}
				else
				{
					// Left integral
					auto res = ll();
					return res <= rhs;
				}
			}
		}

		// Greater than operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_any_numerics<T, xellanix::type::number> = true>
		constexpr bool operator>(T const& rhs) const
		{
			return !operator<=(rhs);
		}

		// Greater than operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		friend constexpr bool operator>(T const lhs, xellanix::type::number const& rhs)
		{
			return rhs < lhs;
		}

		// Greater than or equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_any_numerics<T, xellanix::type::number> = true>
		constexpr bool operator>=(T const& rhs) const
		{
			return !operator<(rhs);
		}

		// Greater than or equal to operators
		// Note: Epsilon factorization will be performed on decimal number comparisons
		template<typename T, helper::enable_if_arithm<T> = true>
		friend constexpr bool operator>=(T const lhs, xellanix::type::number const& rhs)
		{
			return rhs <= lhs;
		}

		// Bitwise operators
		// &, |, ^, ~

		// Additional arithmetic functions
		
		// Division operation on signed type
		// I-------------------------------I
		// 
		// Note: if the result is a decimal number, then the largest number that can be inputted and output without losing accuracy is 2 ^ 53 (9.007.199.254.740.992)
		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		constexpr auto div(SignedT value) const
		{
			constexpr auto is_right_floating = std::is_floating_point_v<SignedT>;

			if constexpr (!is_right_floating)
			{
				if (xellanix::math::equal(value, (SignedT)1))
				{
					return *this;
				}
				else if (xellanix::math::equal(value, (SignedT)0))
				{
					return number{ infinity() };
				}
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();

				if constexpr (is_right_floating)
				{
					auto finalRes = res / value;
					return number{ finalRes };
				}
				else
				{
					return signdiff_division2(res, value);
				}
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res / value;
					return number{ finalRes };
				}
				else
				{
					const auto _ll = ll();

					if constexpr (is_right_floating)
					{
						auto finalRes = _ll / value;
						return number{ finalRes };
					}
					else
					{
						if (!xellanix::math::is_divres_int(_ll, value))
						{
							auto res = d();

							auto finalRes = res / value;
							return number{ finalRes };
						}
						else
						{
							auto finalRes = _ll / value;

							return compress_type(finalRes);
						}
					}
				}
			}
		}

		// Division operation on unsigned type
		// I-------------------------------I
		// 
		// Note: if the result is a decimal number, then the largest number that can be inputted and output without losing accuracy is 2 ^ 53 (9.007.199.254.740.992)
		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		constexpr auto div(UnsignedT valueTemp) const
		{
			using _NonBoolT = typename std::conditional_t<helper::is_same_r_v<UnsignedT, bool>, unsigned char, UnsignedT>;
			_NonBoolT value = valueTemp;

			if (xellanix::math::equal(value, (UnsignedT)1))
			{
				return *this;
			}
			else if (xellanix::math::equal(value, (UnsignedT)0))
			{
				return number{ infinity() };
			}

			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();

				if (!xellanix::math::is_divres_int(res, value))
				{
					auto finalRes = d() / value;
					return number{ finalRes };
				}
				else
				{
					auto finalRes = res / value;

					return compress_type(finalRes);
				}
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();

					auto finalRes = res / value;
					return number{ finalRes };
				}
				else
				{
					auto res = ll();

					return signdiff_division2(res, value);
				}
			}
		}

		// Division operation on signed type and xellanix number
		// I-------------------------------I
		// 
		// Note: if the result is a decimal number, then the largest number that can be inputted and output without losing accuracy is 2 ^ 53 (9.007.199.254.740.992)
		template<typename SignedT, helper::enable_if_signed<SignedT> = true>
		friend constexpr auto div(SignedT lhs_raw, xellanix::type::number const& rhs)
		{
			constexpr auto is_left_floating = std::is_floating_point_v<SignedT>;
			using highest_t = std::conditional_t<is_left_floating, double, long long>;

			highest_t lhs = xellanix::utility::cast_to<highest_t>(lhs_raw);

			if constexpr (!is_left_floating)
			{
				if (xellanix::math::equal(lhs, (highest_t)0))
				{
					return number{};
				}
			}

			const auto ridx = rhs.index();

			if (rhs.is_unsigned(ridx))
			{
				auto res = rhs.ull();

				if constexpr (is_left_floating)
				{
					auto finalRes = lhs / res;
					return number{ finalRes };
				}
				else
				{
					return rhs.signdiff_division2(lhs, res);
				}
			}
			else
			{
				if (rhs.is_floating_point(ridx))
				{
					auto res = rhs.d();

					auto finalRes = lhs / res;
					return number{ finalRes };
				}
				else
				{
					if constexpr (is_left_floating)
					{
						return number{ lhs / rhs.ll() };
					}
					else
					{
						if (auto _ll = rhs.ll(); !xellanix::math::is_divres_int(lhs, _ll))
						{
							auto res = rhs.d();

							auto finalRes = lhs / res;
							return number{ finalRes };
						}
						else
						{
							auto finalRes = lhs / _ll;

							return rhs.compress_type(finalRes);
						}
					}
				}
			}
		}

		// Division operation on unsigned type and xellanix number
		// I-------------------------------I
		// 
		// Note: if the result is a decimal number, then the largest number that can be inputted and output without losing accuracy is 2 ^ 53 (9.007.199.254.740.992)
		template<typename UnsignedT, helper::enable_if_unsigned<UnsignedT> = true>
		friend constexpr auto div(UnsignedT lhs_raw, xellanix::type::number const& rhs)
		{
			auto lhs = xellanix::utility::cast_to<unsigned long long>(lhs_raw);

			if (xellanix::math::equal(lhs, 0ull))
			{
				return number{};
			}

			const auto ridx = rhs.index();

			if (rhs.is_unsigned(ridx))
			{
				auto res = rhs.ull();

				if (!xellanix::math::is_divres_int(lhs, res))
				{
					auto finalRes = lhs / rhs.d();
					return number{ finalRes };
				}
				else
				{
					auto finalRes = lhs / res;

					return rhs.compress_type(finalRes);
				}
			}
			else
			{
				if (rhs.is_floating_point(ridx))
				{
					auto res = rhs.d();

					auto finalRes = lhs / res;
					return number{ finalRes };
				}
				else
				{
					auto res = rhs.ll();

					return rhs.signdiff_division2(lhs, res);
				}
			}
		}

		// Division operation on xellanix number
		// I-------------------------------I
		// 
		// Note: if the result is a decimal number, then the largest number that can be inputted and output without losing accuracy is 2 ^ 53 (9.007.199.254.740.992)
		constexpr auto div(xellanix::type::number const& value) const
		{
			const auto lidx = index();

			if (is_unsigned(lidx))
			{
				auto res = ull();
				return res / value;
			}
			else
			{
				if (is_floating_point(lidx))
				{
					auto res = d();
					return res / value;
				}
				else
				{
					auto res = ll();
					return res / value;
				}
			}
		}

		constexpr auto mod_floor() const
		{

		}

		// Stream operators
		friend decltype(auto) operator<<(std::wostream& out, const number& value)
		{
			value.visit([&](auto&& item)
			{
				using U = typename std::_Remove_cvref_t<decltype(item)>;

				if constexpr (std::is_same_v<U, char>)
				{
					out << (short)item;
				}
				else if constexpr (std::is_same_v<U, unsigned char>)
				{
					out << (unsigned short)item;
				}
				else
				{
					out << item;
				}
			});
			return out;
		}
	};
}
