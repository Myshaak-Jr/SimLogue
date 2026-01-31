#include "util.h"
#include <chrono>
#include <ctime>
#include <format>
#include <string>


std::string make_timestamp() {
	using namespace std::chrono;

	auto now = system_clock::now();
	auto t = system_clock::to_time_t(now);

	std::tm tm{};
	localtime_s(&tm, &t);

	return std::format("{:04}-{:02}-{:02}-{:02}-{:02}-{:02}",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec
	);
}

size_t floor_sqrt(size_t n) {
	size_t lo = 0, hi = n, ans = 0;

	while (lo <= hi) {
		size_t mid = (lo + hi) / 2;
		size_t sq = mid * mid;

		if (sq <= n) {
			ans = mid;
			lo = mid + 1;
		}
		else {
			hi = mid - 1;
		}
	}

	return ans;
}

size_t ceil_sqrt(size_t n) {
	size_t ans = floor_sqrt(n);
	return (ans * ans == n) ? ans : ans + 1;
}