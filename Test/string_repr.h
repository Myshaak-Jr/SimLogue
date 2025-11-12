#pragma once

#include "CppUnitTest.h"

#include <format>

#include "../circuits/src/lingebra/lingebra.h"


using namespace lingebra;


template <ModIntLike T>
std::wstring my_to_string(const T& x);

template <VectorLike T>
std::wstring my_to_string(const T& vector);

template <MatrixLike T>
std::wstring my_to_string(const T& matrix);

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template <> std::wstring ToString<Z_7>(const Z_7& x) { return my_to_string(x); }
	template <> std::wstring ToString<Z_5>(const Z_5& x) { return my_to_string(x); }
	template <> std::wstring ToString<Vector<Z_7>>(const Vector<Z_7>& x) { return my_to_string(x); }
	template <> std::wstring ToString<Matrix<Z_7>>(const Matrix<Z_7>& x) { return my_to_string(x); }
}

// ---------- ModInt<N> ----------
template <ModIntLike T>
std::wstring my_to_string(const T& x) {
	return std::format(L"{} (mod {})", x.get(), x.get_n());
}

// ---------- Vector<N> ----------
template <VectorLike T>
std::wstring my_to_string(const T& vector) {
	using elem_t = T::value_type;
	std::wstringstream ss;
	ss << L"[";
	for (size_t i = 0; i < vector.dim(); ++i) {
		if (i) ss << L", ";
		if constexpr (ModIntLike<elem_t>) {
			ss << Microsoft::VisualStudio::CppUnitTestFramework::ToString(vector[i].get());
		}
		else {
			ss << Microsoft::VisualStudio::CppUnitTestFramework::ToString(vector[i]);
		}
	}
	ss << L"]";
	if constexpr (ModIntLike<elem_t>) {
		ss << L" (mod (" << make_zero<elem_t>().get_n() << L")";
	}

	return ss.str();
}

// ---------- Matrix<N> ----------
template <MatrixLike T>
std::wstring my_to_string(const T& matrix) {
	using elem_t = T::value_type;
	std::wstringstream ss;
	ss << L"[";
	for (size_t i = 0; i < matrix.m(); ++i) {
		if (i) ss << L", ";
		ss << L"[";
		for (size_t j = 0; j < matrix.n(); ++j) {
			if (j) ss << L", ";
			if constexpr (ModIntLike<elem_t>) {
				ss << Microsoft::VisualStudio::CppUnitTestFramework::ToString(matrix(i, j).get());
			}
			else {
				ss << Microsoft::VisualStudio::CppUnitTestFramework::ToString(matrix(i, j));
			}
		}
		ss << L"]";
	}
	ss << L"]";
	if constexpr (ModIntLike<elem_t>) {
		ss << L" (mod (" << make_zero<elem_t>().get_n() << L")";
	}

	return ss.str();
}
