#include <initializer_list>
#include "xellanix_lib.h"

#include <chrono>

#include "temp.h"
#include <functional>

#define TESTCONST const
#define QUICKVIEW constexpr auto

constexpr size_t totalLoop = 10'000'000;
#define IS_SAVE_DATA 0

template<typename L, typename R, typename E>
bool SUM_TEST(L const& left, R const& right, E const expected, int index)
{
	std::map<long long, unsigned int> record;
	bool returnval = true;
	auto lc = print_code(left) + L"_" + print_code(right);

	xellanix::number _2{};

	for (size_t i = 0; i < totalLoop; i++)
	{
		auto start = chrono::high_resolution_clock::now();
		TESTCONST xellanix::number _3 = left + right;

		auto end = chrono::high_resolution_clock::now();

		record_durr(record, start, end, { 100'000u, 75'000u, 50'000u, 40'000u, 30'000u, 20'000u, 10'000u, 5000u, 3000u, 2000u, 1000u });
		// print_progress(i, totalLoop);

		_2 = _3;
	}

	if (_2 != expected)
	{
		if constexpr (std::is_same_v<L, char>)
		{
			wcout << index << L". " << lc << L" -> " << (short)left << L" + " << right << L" = " << _2 << L" (" << expected << L")" << endl;
		}
		else if constexpr (std::is_same_v<R, char>)
		{
			wcout << index << L". " << lc << L" -> " << left << L" + " << (short)right << L" = " << _2 << L" (" << expected << L")" << endl;
		}
		else
		{
			wcout << index << L". " << lc << L" -> " << left << L" + " << right << L" = " << _2 << L" (" << expected << L")" << endl;
		}

		returnval = false;
	}

	#if IS_SAVE_DATA

	save_records(record, totalLoop, L"sum_" + lc);

	#endif // IS_SAVE_DATA

	return returnval;
}

template<typename L, typename R, typename E>
bool SUBTRACT_TEST(L const& left, R const& right, E const expected, int index)
{
	std::map<long long, unsigned int> record;

	bool returnval = true;
	auto lc = print_code(left) + L"_" + print_code(right);

	xellanix::number _2{};

	for (size_t i = 0; i < totalLoop; i++)
	{
		auto start = chrono::high_resolution_clock::now();
		TESTCONST xellanix::number _3 = left - right;

		auto end = chrono::high_resolution_clock::now();

		record_durr(record, start, end, { 100'000u, 75'000u, 50'000u, 40'000u, 30'000u, 20'000u, 10'000u, 5000u, 3000u, 2000u, 1000u });
		// print_progress(i, totalLoop);

		_2 = _3;
	}

	if (_2 != expected)
	{
		if constexpr (std::is_same_v<L, char>)
		{
			wcout << index << L". " << lc << L" -> " << (short)left << L" - " << right << L" = " << _2 << L" (" << expected << L")" << endl;
		}
		else if constexpr (std::is_same_v<R, char>)
		{
			wcout << index << L". " << lc << L" -> " << left << L" - " << (short)right << L" = " << _2 << L" (" << expected << L")" << endl;
		}
		else
		{
			wcout << index << L". " << lc << L" -> " << left << L" - " << right << L" = " << _2 << L" (" << expected << L")" << endl;
		}

		returnval = false;
	}

	#if IS_SAVE_DATA

	save_records(record, totalLoop, L"subtract" + lc);

	#endif // IS_SAVE_DATA

	return returnval;
}

template<typename L, typename R, typename E>
bool MULTIPLY_TEST(L const& left, R const& right, E const expected, int index)
{
	std::map<long long, unsigned int> record;
	bool returnval = true;
	auto lc = print_code(left) + L"_" + print_code(right);

	xellanix::number _2{};

	for (size_t i = 0; i < totalLoop; i++)
	{
		auto start = chrono::high_resolution_clock::now();
		TESTCONST xellanix::number _3 = left * right;

		auto end = chrono::high_resolution_clock::now();

		record_durr(record, start, end, { 100'000u, 75'000u, 50'000u, 40'000u, 30'000u, 20'000u, 10'000u, 5000u, 3000u, 2000u, 1000u });
		// print_progress(i, totalLoop);

		_2 = _3;
	}

	if (_2 != expected)
	{
		if constexpr (std::is_same_v<L, char>)
		{
			wcout << index << L". " << lc << L" -> " << (short)left << L" * " << right << L" = " << _2 << L" (" << expected << L")" << endl;
		}
		else if constexpr (std::is_same_v<R, char>)
		{
			wcout << index << L". " << lc << L" -> " << left << L" * " << (short)right << L" = " << _2 << L" (" << expected << L")" << endl;
		}
		else
		{
			wcout << index << L". " << lc << L" -> " << left << L" * " << right << L" = " << _2 << L" (" << expected << L")" << endl;
		}

		returnval = false;
	}

	#if IS_SAVE_DATA

	save_records(record, totalLoop, L"multiply_" + lc);

	#endif // IS_SAVE_DATA

	return returnval;
}

template<typename Fn, typename Tuple, std::size_t... Is>
void OperateTuple(const Tuple& left, std::index_sequence<Is...>, Fn&& fn)
{
	(fn(std::get<Is>(left)), ...);
}

template<typename Tuple, std::size_t tsize = std::tuple_size_v<Tuple>, typename Fn>
void OperateTuple(const Tuple& left, Fn&& fn)
{
	OperateTuple(left, std::make_index_sequence<tsize>{}, std::forward<Fn>(fn));
}

int main()
{
	constexpr auto left_posibility = std::make_tuple(-2, 2, 2u, 2.0, 0, false, true,
													 xellanix::nmin<char>(),
													 xellanix::nmax<char>(),
													 xellanix::nmin<unsigned char>(),
													 xellanix::nmax<unsigned char>(),
													 xellanix::nmin<short>(),
													 xellanix::nmax<short>(),
													 xellanix::nmin<unsigned short>(),
													 xellanix::nmax<unsigned short>(),
													 xellanix::nmin<int>(),
													 xellanix::nmax<int>(),
													 xellanix::nmin<unsigned int>(),
													 xellanix::nmax<unsigned int>(),
													 xellanix::nmin<long long>(),
													 xellanix::nmax<long long>(),
													 xellanix::nmin<unsigned long long>(),
													 xellanix::nmax<unsigned long long>(),
													 xellanix::nmin<float>(),
													 xellanix::nmax<float>(),
													 xellanix::nmin<double>(),
													 xellanix::nmax<double>());

	/*OperateTuple(left_posibility, [](const auto& value)
	{
		OperateTuple(left_posibility, [value](const auto& value2)
		{
			const auto expected = value + value2;

			auto _ = SUM_TEST(xellanix::number{ value }, xellanix::number{ value2 }, expected, 1);
			_ = _ && SUM_TEST(xellanix::number{ value }, value2, expected, 2);
			_ = _ && SUM_TEST(value, xellanix::number{ value2 }, expected, 3);
			
			if (!_) wcout << endl;
		});
	});*/

	#define TEST_RUNNER MULTIPLY_TEST

	OperateTuple(left_posibility, [](const auto& value)
	{
		OperateTuple(left_posibility, [value](const auto& value2)
		{
			const auto expected = value * value2;

			auto _ = TEST_RUNNER(xellanix::number{ value }, xellanix::number{ value2 }, expected, 1);
			_ = _ && TEST_RUNNER(xellanix::number{ value }, value2, expected, 2);
			_ = _ && TEST_RUNNER(value, xellanix::number{ value2 }, expected, 3);

			if (!_) wcout << endl;
		});
	});

	#if IS_SAVE_DATA

	package_records();

	#endif // IS_SAVE_DATA

	// print_records(record, totalLoop);

	return 0;
}
