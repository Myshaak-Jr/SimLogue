#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace lingebra {
	// generic abs
	using std::abs;

	template <class T>
	concept has_abs_method = requires (const T & a) { { a.abs() } -> std::convertible_to<T>; };

	template <class T>
	concept has_abs_static_method = requires (const T & a) { { T::abs(a) } -> std::convertible_to<T>; };

	template <has_abs_method T>
	constexpr T abs(const T &x) {
		return x.abs();
	}

	template <has_abs_static_method T>
	constexpr T abs(const T &x) {
		return T::abs(x);
	}

	// is prime
	constexpr bool is_prime(size_t n) noexcept {
		if (n < 2) return false;
		for (size_t i = 2; i * i <= n; ++i) {
			if (n % i == 0) return false;
		}
		return true;
	}


	// modulo integer

	template <size_t N>
		requires (N > 1)
	class ModInt {
	public:
		using value_type =
			std::conditional_t<(N <= 127), int8_t,
			std::conditional_t<(N <= 32767), int16_t,
			std::conditional_t<(N <= 2147483647), int32_t, int64_t>>>;

	private:
		value_type value;
		static constexpr value_type n = static_cast<value_type>(N);

	public:
		constexpr ModInt(value_type value = 0) noexcept : value(((value %n) + n) % n) {}

		template <class TEngine>
		static ModInt make_random(TEngine &engine) {
			std::uniform_int_distribution<uint64_t> dist(0, static_cast<uint16_t>(get_n()) - 1);
			return ModInt(static_cast<value_type>(dist(engine)));
		}

		constexpr value_type get() const noexcept { return value; }
		constexpr static value_type get_n() noexcept { return n; }

		friend auto operator<=>(const ModInt &, const ModInt &) = default;

		constexpr ModInt &operator+=(const ModInt &other) noexcept {
			value += other.value;
			if (value >= n) value -= n;
			return *this;
		}
		constexpr ModInt &operator-=(const ModInt &other) noexcept {
			value -= other.value;
			if (value < 0) value += n;
			return *this;
		}
		constexpr ModInt &operator*=(const ModInt &other) noexcept {
			value = (static_cast<uint64_t>(value) * other.value) % n;
			return *this;
		}

		constexpr ModInt &operator++() noexcept {
			if (++value == n) value = 0;
			return *this;
		}
		constexpr ModInt operator++(int) noexcept {
			ModInt temp = *this;
			++(*this);
			return temp;
		}

		constexpr ModInt operator+(const ModInt &other) const noexcept {
			ModInt temp = *this;
			temp += other;
			return temp;
		}
		constexpr ModInt operator-(const ModInt &other) const noexcept {
			ModInt temp = *this;
			temp -= other;
			return temp;
		}
		constexpr ModInt operator*(const ModInt &other) const noexcept {
			ModInt temp = *this;
			temp *= other;
			return temp;
		}

		constexpr ModInt abs() const noexcept {
			return ModInt(*this);
		}

		// Prime N only

		// uses Extended Euclidian
		// Undefined behavior if called on zero.
		constexpr ModInt inverse() const noexcept requires (is_prime(N)) {
			value_type t = 0;
			value_type new_t = 1;
			value_type r = n;
			value_type new_r = value;

			while (new_r != 0) {
				const value_type q = r / new_r;
				const value_type temp_r = r - q * new_r;
				r = new_r;
				new_r = temp_r;
				const value_type temp_t = t - q * new_t;
				t = new_t;
				new_t = temp_t;
			}

			return ModInt(t);
		}

		constexpr ModInt &operator/=(const ModInt &other) noexcept requires (is_prime(N)) {
			(*this) *= other.inverse();
			return *this;
		}

		constexpr ModInt operator/(const ModInt &other) noexcept requires (is_prime(N)) {
			ModInt temp = *this;
			temp /= other;
			return temp;
		}
	};

	using Z_2 = ModInt<2>;
	using Z_3 = ModInt<3>;
	using Z_5 = ModInt<5>;
	using Z_7 = ModInt<7>;

	template <class T> struct is_ModInt : std::false_type {};
	template <size_t N> struct is_ModInt<ModInt<N>> : std::true_type {};
	template <class T> inline constexpr bool is_ModInt_v = is_ModInt<T>::value;
	template <class T> concept ModIntLike = is_ModInt_v<T>;

	// "field" concept

	template <class T>
	concept has_zero =
		requires { { T::zero() } -> std::convertible_to<T>; } ||
		requires { { T::zero } -> std::convertible_to<T>; } ||
	std::constructible_from<T, int>;

	template <has_zero T>
	consteval T make_zero() {
		if constexpr (requires { { T::zero() } -> std::convertible_to<T>; }) {
			return T::zero();
		}
		else if constexpr (requires { { T::zero } -> std::convertible_to<T>; }) {
			return static_cast<T>(T::zero);
		}
		else {
			static_assert(std::constructible_from<T, int>);
			return T{ 0 };
		}
	}

	template <has_zero T>
	constexpr bool is_zero(const T &a) {
		if constexpr (requires (T x) { { T::is_zero(x) } -> std::convertible_to<bool>; }) {
			return T::is_zero(a);
		}
		else if constexpr (requires (T x) { { x.is_zero() } -> std::convertible_to<bool>; }) {
			return a.is_zero();
		}
		else if constexpr (std::floating_point<T>) {
			return (std::fabs(a) < std::numeric_limits<T>::epsilon());
		}
		else {
			return (a == make_zero<T>());
		}
	}

	template <class T>
	concept has_one =
		requires { { T::one() } -> std::convertible_to<T>; } ||
		requires { { T::one } -> std::convertible_to<T>; } ||
	std::constructible_from<T, int>;

	template <has_one T>
	consteval T make_one() {
		if constexpr (requires { { T::one() } -> std::convertible_to<T>; }) {
			return T::one();
		}
		else if constexpr (requires { { T::one } -> std::convertible_to<T>; }) {
			return static_cast<T>(T::one);
		}
		else {
			static_assert(std::constructible_from<T, int>);
			return T{ 1 };
		}
	}

	template <class T>
	concept additive =
		requires (T a, T b) { { a + b } -> std::convertible_to<T>; } &&
		requires (T a, T b) { { b + a } -> std::convertible_to<T>; } &&
		requires (T a, T b) { { a - b } -> std::convertible_to<T>; } &&
		requires (T a, T b) { { b - a } -> std::convertible_to<T>; };

	template <class T>
	concept multiplicative =
		requires (T a, T b) { { a *b } -> std::convertible_to<T>; } &&
		requires (T a, T b) { { b *a } -> std::convertible_to<T>; } &&
		requires (T a, T b) { { a / b } -> std::convertible_to<T>; } &&
		requires (T a, T b) { { b / a } -> std::convertible_to<T>; };

	template <class T>
	concept field = additive<T> && multiplicative<T> &&
		requires { make_one<T>() != make_zero<T>(); };


	// Vectors and Matrices
	template <field F>
	class Vector {
	public:
		using value_type = F;

	private:
		std::vector<F> data;

		static constexpr F zero = make_zero<F>();

	public:
		constexpr Vector() noexcept = default;
		constexpr Vector(size_t size, const F &value) : data(size, value) {}
		constexpr explicit Vector(size_t size) : Vector(size, zero) {}
		constexpr Vector(std::initializer_list<F> list) : data(list) {}
		constexpr Vector(const std::vector<F> &data) : data(data) {}
		template <std::input_iterator Iter>
		constexpr Vector(Iter first, Iter last) : data(first, last) {}

		constexpr Vector(const Vector &) = default;
		constexpr Vector(Vector &&) noexcept = default;
		constexpr Vector &operator=(const Vector &) = default;
		constexpr Vector &operator=(Vector &&) noexcept = default;

		~Vector() = default;


		// random generation (only for finite division rings)
		template <class TEngine>
		static Vector make_random(TEngine &engine, size_t size) requires ModIntLike<F> {
			Vector vec(size);
			for (size_t i = 0; i < size; ++i) {
				vec[i] = F::make_random(engine);
			}
			return vec;
		}

		constexpr void clear() {
			assign(data.size(), zero);
		}

		constexpr void assign(size_t new_size, const F &value = zero) {
			data.assign(new_size, value);
		}

		constexpr F &operator[](size_t pos) noexcept {
			return data[pos];
		}

		constexpr const F &operator[](size_t pos) const noexcept {
			return data[pos];
		}

		constexpr size_t dim() const noexcept {
			return data.size();
		}

		constexpr void swap_values(size_t a, size_t b) noexcept(std::is_nothrow_swappable_v<F>) {
			using std::swap;
			swap(data[a], data[b]);
		}

		constexpr std::string repr() const {
			std::stringstream ss;
			for (size_t i = 0; i < data.size(); ++i) {
				ss << (i == 0 ? "[" : ", ");

				if constexpr (std::floating_point<F>) {
					ss << std::format("{:.3f}", data[i]);
				}
				else {
					ss << data[i];
				}
			}
			ss << "]";

			return ss.str();
		}

		constexpr bool operator==(const Vector &other) const {
			return data == other.data;
		}

		constexpr Vector &operator+=(const Vector &other) {
			if (dim() != other.dim()) {
				throw std::runtime_error("Vector must be of the same dimension");
			}

			for (size_t i = 0; i < dim(); ++i) {
				data[i] += other[i];
			}

			return *this;
		}

		constexpr Vector operator+(const Vector &other) const {
			Vector result(*this);
			result += other;
			return result;
		}

		constexpr F operator*(const Vector &other) const {
			if (dim() != other.dim()) {
				throw std::runtime_error("Vector must be of the same dimension");
			}

			F sum = make_zero<F>();

			for (size_t i = 0; i < dim(); ++i) {
				sum += data[i] * other[i];
			}

			return sum;
		}

		constexpr Vector &operator*=(const F &scalar) {
			for (size_t i = 0; i < dim(); ++i) {
				data[i] *= scalar;
			}
			return *this;
		}
		constexpr Vector &operator/=(const F &scalar) {
			for (size_t i = 0; i < dim(); ++i) {
				data[i] /= scalar;
			}
			return *this;
		}

		constexpr Vector operator*(const F &scalar) const {
			Vector temp(*this);
			temp *= scalar;
			return temp;
		}
		constexpr friend Vector operator*(const F &scalar, const Vector &vec) {
			return vec * scalar;
		}

		constexpr Vector operator/(const F &scalar) const {
			Vector temp(*this);
			temp /= scalar;
			return temp;
		}
	};


	// A dense matrix
	template <field F>
	class Matrix {
	public:
		using value_type = F;

	private:
		std::vector<std::vector<F>> data;
		size_t num_rows, num_cols;

		static constexpr F zero = make_zero<F>();

	public:
		constexpr Matrix(const std::vector<std::vector<F>> &data) : data(data) {
			num_rows = data.size();
			if (num_rows != 0) {
				num_cols = data[0].size();
			}
			else {
				num_cols = 0;
			}
		}
		constexpr Matrix() noexcept : num_rows(0), num_cols(0) {}
		constexpr Matrix(size_t m, size_t n, const F &value) : data(m, std::vector<F>(n, value)), num_rows(m), num_cols(n) {}
		constexpr Matrix(size_t m, size_t n) : Matrix(m, n, zero) {}
		constexpr Matrix(std::initializer_list<std::initializer_list<F>> list) : num_cols(0) {
			num_rows = list.size();
			bool num_cols_set = false;
			for (const std::initializer_list<F> &row : list) {
				data.push_back(row);
				if (!num_cols_set) {
					num_cols = row.size();
					num_cols_set = true;
				}
				else {
					if (row.size() != num_cols) {
						throw std::runtime_error("All rows must be of the same lenght!");
					}
				}
			}
		}

		constexpr Matrix(const Matrix &) = default;
		constexpr Matrix(Matrix &&) noexcept = default;
		constexpr Matrix &operator=(const Matrix &) = default;
		constexpr Matrix &operator=(Matrix &&) noexcept = default;

		~Matrix() = default;


		// random generation (only for finite division rings)
		template <class TEngine>
		static Matrix make_random(TEngine &engine, size_t m, size_t n) requires ModIntLike<F> {
			Matrix mat(m, n);
			for (size_t i = 0; i < m; ++i) {
				for (size_t j = 0; j < n; ++j) {
					mat(i, j) = F::make_random(engine);
				}
			}
			return mat;
		}


		constexpr void clear() {
			assign(num_rows, num_cols, zero);
		}

		constexpr void assign(size_t m, size_t n, const F &value = zero) {
			num_rows = m;
			num_cols = n;
			data.assign(m, std::vector<F>(n, value));
		}

		constexpr F &operator()(size_t row, size_t col) noexcept {
			return data[row][col];
		}

		constexpr const F &operator()(size_t row, size_t col) const noexcept {
			return data[row][col];
		}

		constexpr std::vector<std::vector<F>> &rows() noexcept {
			return data;
		}

		constexpr const std::vector<std::vector<F>> &rows() const noexcept {
			return data;
		}

		constexpr size_t m() const noexcept {
			return num_rows;
		}

		constexpr size_t n() const noexcept {
			return num_cols;
		}

		constexpr std::string repr() const {
			size_t max_length = 0;
			for (size_t i = 0; i < num_rows; ++i) {
				for (size_t j = 0; j < num_cols; ++j) {
					auto len = std::to_string(data[i][j]).length();
					max_length = std::max(len, max_length);
				}
			}

			std::stringstream ss;
			for (size_t i = 0; i < num_rows; ++i) {
				ss << (i == 0 ? "[" : " ");

				for (size_t j = 0; j < num_cols; ++j) {
					ss << (j == 0 ? "[" : " ");

					if constexpr (std::floating_point<F>) {
						ss << std::format("{:> {}.3f}", data[i][j], max_length);
					}
					else {
						ss << std::format("{:> {}}", data[i][j], max_length);
					}
				}

				ss << "]";
				if (i < num_rows - 1) ss << "\n";
			}
			ss << "]";

			return ss.str();
		}

		constexpr void swap_rows(size_t a, size_t b) noexcept(std::is_nothrow_swappable_v<std::vector<F>>) {
			using std::swap;
			swap(data[a], data[b]);
		}

		constexpr bool is_square() const noexcept {
			return num_rows == num_cols;
		}

		constexpr bool operator==(const Matrix &other) const {
			return data == other.data;
		}

		constexpr Matrix &operator+=(const Matrix &other) {
			if (num_rows != other.num_rows || num_cols != other.num_cols) {
				throw std::runtime_error("Matrices must be of the same size");
			}

			for (size_t i = 0; i < num_rows; ++i) {
				for (size_t j = 0; j < num_cols; ++j) {
					data[i][j] += other(i, j);
				}
			}

			return *this;
		}

		constexpr Matrix operator+(const Matrix &other) const {
			Matrix result(*this);
			result += other;
			return result;
		}

		constexpr Matrix operator*(const Matrix &other) const {
			if (num_cols != other.num_rows) {
				throw std::runtime_error("Uncompatible matrices for matrix product");
			}

			Matrix result(num_rows, other.num_cols);

			for (size_t i = 0; i < num_rows; ++i) {
				for (size_t j = 0; j < other.num_cols; ++j) {
					F sum = make_zero<F>();

					for (size_t k = 0; k < num_cols; ++k) {
						sum += (*this)(i, k) * other(k, j);
					}

					result(i, j) = sum;
				}
			}

			return result;
		}

		constexpr Matrix &operator*=(const F &scalar) {
			for (size_t i = 0; i < num_rows; ++i) {
				for (size_t j = 0; j < num_cols; ++j) {
					(*this)(i, j) *= scalar;
				}
			}
			return *this;
		}
		constexpr Matrix &operator/=(const F &scalar) {
			for (size_t i = 0; i < num_rows; ++i) {
				for (size_t j = 0; j < num_cols; ++j) {
					(*this)(i, j) /= scalar;
				}
			}
			return *this;
		}

		constexpr Matrix operator*(const F &scalar) const {
			Matrix temp(*this);
			temp *= scalar;
			return temp;
		}
		constexpr friend Matrix operator*(const F &scalar, const Matrix &vec) {
			return vec * scalar;
		}

		constexpr Matrix operator/(const F &scalar) const {
			Matrix temp(*this);
			temp /= scalar;
			return temp;
		}
	};


	template <field F>
	Vector<F> operator*(const Matrix<F> &matrix, const Vector<F> &vector) {
		if (matrix.n() != vector.dim()) {
			throw std::runtime_error("Uncompatible matrix, vector size");
		}

		Vector<F> result(matrix.m());

		for (size_t i = 0; i < matrix.m(); ++i) {
			F sum = make_zero<F>();
			for (size_t j = 0; j < matrix.n(); ++j) {
				sum += matrix(i, j) * vector[j];
			}
			result[i] = sum;
		}

		return result;
	}

	template <field F>
	Vector<F> operator*(const Vector<F> &vector, const Matrix<F> &matrix) {
		if (matrix.m() != vector.dim()) {
			throw std::runtime_error("Uncompatible matrix, vector size");
		}

		Vector<F> result(matrix.n());

		for (size_t i = 0; i < matrix.n(); ++i) {
			F sum = make_zero<F>();
			for (size_t j = 0; j < matrix.m(); ++j) {
				sum += vector[j] * matrix(j, i);
			}
			result[i] = sum;
		}

		return result;
	}


	class singular_matrix_exception : public std::runtime_error {
	public:
		explicit singular_matrix_exception() : runtime_error("The matrix is singular!") {}
	};

	/* Solves the system Ax = b using the gaussian elimination */
	template <field F>
	void solve_gaussian_elimination(Matrix<F> &matrix, Vector<F> &b) {
		size_t n = matrix.n();
		if (b.dim() != n)
			throw std::runtime_error("Size mismatch in solve_gaussian_elimination");

		size_t m = matrix.m();



		size_t h = 0;
		size_t k = 0;

		while (h < m && k < n) {
			// find k-th pivot
			size_t i_max = h;
			for (size_t i = h + 1; i < m; ++i) {
				if (matrix(i, k) > matrix(i_max, k)) i_max = i;
			}
			if (is_zero(matrix(i_max, k))) {
				// no pivot in column => singular matrix
				throw singular_matrix_exception();
			}

			matrix.swap_rows(h, i_max);
			b.swap_values(h, i_max);
			F global_f = make_one<F>() / matrix(h, k);

			// make the pivot equal one
			matrix(h, k) = make_one<F>();
			for (size_t j = k + 1; j < n; ++j) {
				matrix(h, j) *= global_f;
			}
			b[h] *= global_f;

			// eliminate
			for (size_t i = 0; i < m; ++i) {
				if (i == h) continue;
				F local_f = matrix(i, k);
				matrix(i, k) = make_zero<F>();
				for (size_t j = k + 1; j < n; ++j) {
					matrix(i, j) -= matrix(h, j) * local_f;
				}
				b[i] -= b[h] * local_f;
			}

			++h;
			++k;
		}
	}

	// type traits and concepts for vectors and matrices
	template <class T> struct is_Vector : std::false_type {};
	template <class T> struct is_Vector<Vector<T>> : std::true_type {};
	template <class T> inline constexpr bool is_Vector_v = is_Vector<T>::value;
	template <class T> concept VectorLike = is_Vector_v<T>;

	template <class T> struct is_Matrix : std::false_type {};
	template <class T> struct is_Matrix<Matrix<T>> : std::true_type {};
	template <class T> inline constexpr bool is_Matrix_v = is_Matrix<T>::value;
	template <class T> concept MatrixLike = is_Matrix_v<T>;
}