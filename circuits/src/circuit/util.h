#pragma once

#include <string>


std::string make_timestamp();


template <class T, class Map>
std::vector<T> counting_sort(std::vector<T>& vec, size_t range, Map map) {
	std::vector<size_t> counts;
	counts.resize(range, 0);

	for (const auto& x : vec) {
		++counts[map(x)];
	}

	std::vector<size_t> begins;
	begins.resize(range, 0);

	for (size_t i = 1; i < begins.size(); ++i) {
		begins[i] = begins[i - 1] + counts[i - 1];
	}

	std::vector<T> result;
	result.resize(vec.size());
	for (size_t i = 0; i < vec.size(); ++i) {
		size_t idx = begins[map(vec[i])]++;
		result[idx] = vec[i];
	}

	return result;
}

template <class T>
std::vector<T> counting_sort(std::vector<T>& vec, size_t range) {
	return counting_sort(vec, range, [](const auto& x) { return static_cast<size_t>(x); })
}