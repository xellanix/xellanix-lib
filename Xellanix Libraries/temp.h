#pragma once

#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include "formatter.h"
#include <Windows.h>
#include <filesystem>
#include <fstream>

using namespace std;

template<typename T>
inline constexpr decltype(auto) nmin()
{
	return (std::numeric_limits<T>::min)();
}

template<typename T>
inline constexpr decltype(auto) nmax()
{
	return (std::numeric_limits<T>::max)();
}

template<typename T>
inline constexpr decltype(auto) nlim()
{
	return std::pair{ nmin<T>(), nmax<T>() };
}

template<typename L, typename R>
inline constexpr decltype(auto) operator==(std::pair<L, L> lhs, std::pair<R, R> rhs)
{
	return std::pair{ lhs.first == rhs.first, lhs.second == rhs.second };
}

inline void record_durr(std::map<long long, unsigned int>& record, std::chrono::steady_clock::time_point const& start, std::chrono::steady_clock::time_point const& end, std::initializer_list<unsigned int> const& group)
{
	auto durr = (end - start).count();

	for (auto& i : group)
	{
		if (durr >= i)
		{
			durr = i;
			break;
		}
	}

	record[durr]++;
}

inline constexpr int num_digits(unsigned long long x)
{
	return x < 10 ? 1 : (1 + num_digits(x / 10));
}

template<typename T1, typename T2>
inline void print_records(std::map<T1, T2> const& record, size_t totalLoop)
{
	wcout << endl << endl;

	double totalTime = 0;
	wostringstream wis;

	for (auto&& [key, val] : record)
	{
		wis << left << setw(5) << key << L" =  " << setw(8) << fixed << setprecision(4) << (val * 100.0 / totalLoop);
		wis << L" (" << val << L")" << endl;

		totalTime += key * val / 1'000.0;
	}

	auto seconds = totalTime / 1'000'000.0;

	wcout << xellanix::formatter::add_num_sep(totalTime) << L" microseconds";
	wcout << L" (" << xellanix::formatter::add_num_sep(seconds) << L" seconds)";
	wcout << endl << endl;
	wcout << wis.str();
}

inline void print_progress(size_t i, size_t totalLoop)
{
	constexpr unsigned long long _1 = 1'000;
	constexpr unsigned long long _2 = 20;
	const auto _ = totalLoop / _1;
	constexpr unsigned long long __ = _1 / _2;
	constexpr unsigned long long __2 = num_digits(_2);

	if (auto adjust = (i + 1); adjust % _ == 0)
	{
		if ((i - (_ - 1)) % (_ * __) == 0)
		{
			auto ___ = (adjust - _) / (_ * __) + 1;
			wcout << endl << setw(__2) << ___ << L". ";
		}
		wcout << L"=";
	}

}

inline constexpr auto is_safe_sum(long long a, long long b)
{
	if (((b > 0) && (a > ((std::numeric_limits<long long>::max)() - b))) ||
		((b < 0) && (a < ((std::numeric_limits<long long>::min)() - b))))
	{
		return false;
	}

	return true;
}

inline constexpr auto is_safe_sub(long long a, long long b)
{
	if (((b > 0) && (a < ((std::numeric_limits<long long>::min)() + b))) ||
		((b < 0) && (a > ((std::numeric_limits<long long>::max)() + b))))
	{
		return false;
	}

	return true;
}

inline std::wstring GetAppDir()
{
	DWORD path_buffer_size = MAX_PATH;
	std::unique_ptr<WCHAR[]> path_buf{ new WCHAR[path_buffer_size] };

	while (true)
	{
		const auto bytes_written = GetModuleFileName(NULL, path_buf.get(), path_buffer_size);
		const auto last_error = GetLastError();

		if (bytes_written == 0)
		{
			return std::wstring{};
		}

		if (last_error == ERROR_INSUFFICIENT_BUFFER)
		{
			path_buffer_size *= 2;
			path_buf.reset(new WCHAR[path_buffer_size]);
			continue;
		}

		auto path = std::wstring{ path_buf.get() };
		if (auto found = path.find_last_of(L"/\\"); found != std::wstring::npos)
		{
			path = path.substr(0, found);
		}

		return path;
	}
}

template<typename T1, typename T2>
inline void save_records(std::map<T1, T2> const& record, size_t totalLoop, std::wstring_view const name)
{
	{
		wostringstream wis;
		auto cvt = std::make_unique<xellanix::cvt::utf8cvt<wchar_t>>();
		wis.imbue(std::locale(std::locale("C"), cvt.release()));

		for (auto&& [key, val] : record)
		{
			wis << key << L" = ";
			wis << val << endl;
		}

		std::wofstream wof;
		wof.imbue(std::locale(std::locale("C"), cvt.release()));

		wof.open(std::filesystem::path(GetAppDir()) / L"TEST_RESULT" / name, std::ios::binary);
		wof << wis.str();
		wof.close();
	}
}

inline void package_records()
{
	ShellExecute(NULL, NULL, LR"(D:\Beta Projects\Xellanix Libraries\ExcelHelper\bin\Release\net7.0\ExcelHelper.exe)",
				 NULL, NULL, SW_SHOWNORMAL);
}

template<typename T>
inline std::wstring print_code(T const& value)
{
	std::wstring wv;

	if constexpr (std::is_same_v<T, xellanix::number>)
	{
		if (value.is_signed())
		{
			if (value.is_negative())
			{
				wv = L"xns";
			}
			else
			{
				wv = L"xps";
			}
		}
		else if (value.is_unsigned())
		{
			wv = L"xpu";
		}
		else if (value.is_floating_point())
		{
			std::wstring n_ = value.is_negative() ? L"n" : L"p";

			if (value.index() == 8)
			{
				wv = L"x" + n_ + L"f";
			}
			else if (value.index() == 9)
			{
				wv = L"x" + n_ + L"d";
			}
		}
	}
	else if constexpr (std::is_integral_v<T>)
	{
		if constexpr (std::is_signed_v<T>)
		{
			std::wstring n_ = value < 0 ? L"n" : L"p";
			wv = L"n" + n_ + L"s";
		}
		else
		{
			wv = L"npu";
		}
	}
	else
	{
		std::wstring n_ = xellanix::math::less(value, 0.0) ? L"n" : L"p";
		if constexpr (std::is_same_v<T, float>)
		{
			wv = L"n" + n_ + L"f";
		}
		else
		{
			wv = L"n" + n_ + L"d";
		}
	}

	return wv;
}