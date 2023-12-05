#pragma once

#ifndef XELLANIX_FORMATTER_H
#define XELLANIX_FORMATTER_H
#endif // !XELLANIX_FORMATTER_H


#ifndef XELLANIX_LOCALE_CVT_H
#include "locale_cvt.h"
#endif // !XELLANIX_LOCALE_CVT_H

#include <string>
#include <sstream>
#include <memory>
#include <iomanip>

namespace xellanix::formatter
{
	struct thousand_sep : std::numpunct<wchar_t>
	{
		char_type do_thousands_sep() const override { return L','; }
		std::string do_grouping() const override { return "\3"; }
	};

	template<typename T>
	inline const std::enable_if_t<std::is_floating_point_v<T>, std::wstring> add_num_sep(T num, std::streamsize decimal_points = 2)
	{
		std::wostringstream wss;
		auto thousands = std::make_unique<thousand_sep>();
		auto cvt = std::make_unique<xellanix::cvt::utf8cvt<wchar_t>>();

		wss.imbue(std::locale(std::locale(std::locale("C"), cvt.release()), thousands.release()));
		wss << std::setprecision(decimal_points) << std::fixed << num;
		return wss.str();
	}

	template<typename T>
	inline const std::enable_if_t<std::is_integral_v<T>, std::wstring> add_num_sep(T num)
	{
		std::wostringstream wss;
		auto thousands = std::make_unique<thousand_sep>();
		auto cvt = std::make_unique<xellanix::cvt::utf8cvt<wchar_t>>();

		wss.imbue(std::locale(std::locale(std::locale("C"), cvt.release()), thousands.release()));
		wss << num;
		return wss.str();
	}
}