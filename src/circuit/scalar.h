#pragma once


#ifdef HIGH_PRECISION
using scalar = double;
#else
using scalar = float;
#endif // HIGH_PRECISION


constexpr scalar operator""_k(long double v) {
	return static_cast<scalar>(v) * 1e3;
}

constexpr scalar operator""_M(long double v) {
	return static_cast<scalar>(v) * 1e6;
}

constexpr scalar operator""_G(long double v) {
	return static_cast<scalar>(v) * 1e9;
}

constexpr scalar operator""_m(long double v) {
	return static_cast<scalar>(v) * 1e-3;
}

constexpr scalar operator""_u(long double v) {
	return static_cast<scalar>(v) * 1e-6;
}

constexpr scalar operator""_n(long double v) {
	return static_cast<scalar>(v) * 1e-9;
}

constexpr scalar operator""_p(long double v) {
	return static_cast<scalar>(v) * 1e-12;
}

constexpr scalar operator""_k(unsigned long long v) {
	return static_cast<scalar>(v) * 1e3;
}

constexpr scalar operator""_M(unsigned long long v) {
	return static_cast<scalar>(v) * 1e6;
}

constexpr scalar operator""_G(unsigned long long v) {
	return static_cast<scalar>(v) * 1e9;
}

constexpr scalar operator""_m(unsigned long long v) {
	return static_cast<scalar>(v) * 1e-3;
}

constexpr scalar operator""_u(unsigned long long v) {
	return static_cast<scalar>(v) * 1e-6;
}

constexpr scalar operator""_n(unsigned long long v) {
	return static_cast<scalar>(v) * 1e-9;
}

constexpr scalar operator""_p(unsigned long long v) {
	return static_cast<scalar>(v) * 1e-12;
}
