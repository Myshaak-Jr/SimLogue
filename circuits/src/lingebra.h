#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>


namespace lingebra {
// ==== Automatic Absolute Value Deduction ====
using std::abs;

template <class T>
concept has_abs_method = requires (const T & a) { { a.abs() } -> std::convertible_to<T>; };

template <class T>
concept has_abs_static_method = requires (const T & a) { { T::abs(a) } -> std::convertible_to<T>; };

template <has_abs_method T>
constexpr T abs(const T &x) { return x.abs(); }

template <has_abs_static_method T>
constexpr T abs(const T &x) { return T::abs(x); }

// ==== Prime Value Checker ====

/// @brief determines whether n is a prime.
/// 
/// Possibly at compile time, runs in O(log n)
constexpr bool is_prime(size_t n) noexcept {
	if (n < 2) return false;
	for (size_t i = 2; i * i <= n; ++i) {
		if (n % i == 0) return false;
	}
	return true;
}


/// @brief Integer arithmetic modulo N.
///
/// Represents an integer value in the ring Z_n.
/// The stored value is always normalized to the range:
///     0 <= value < N
///
/// @tparam N Modulus (must be greater than 1).
///
/// ### Notes
/// - For prime N, division is supported.
/// - For non-prime N, division is intentionally disabled at compile time.
/// - The internal storage type is chosen automatically to fit N efficiently.
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

	/// @brief Generates a uniformly distributed random modulo value.
	///
	/// @tparam TEngine A uniform random bit generator.
	/// @param engine Random engine instance.
	///
	/// @return Random ModInt in range [0, N).
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

	/// @brief Computes the multiplicative inverse.
	///
	/// @pre N must be prime.
	/// @pre value != 0
	///
	/// Uses the Extended Euclidean Algorithm.
	///
	/// @return Multiplicative inverse of this value modulo N.
	///
	/// @warning Calling this on zero results in undefined behavior.
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
template <class T> constexpr bool is_ModInt_v = is_ModInt<T>::value;
template <class T> concept ModIntLike = is_ModInt_v<T>;


// ==== Automatic Ones and Zeros ====

template <class T>
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

template <class T>
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


// ==== Vectors and Matrices ====

/// @brief An arithmetic vector
template <class F>
class Vector {
public:
	using value_type = F;

private:
	std::vector<F> data;

	static constexpr F zero = make_zero<F>();

public:
	Vector() noexcept = default;
	Vector(size_t size, const F &value) : data(size, value) {}
	explicit Vector(size_t size) : Vector(size, zero) {}
	Vector(std::initializer_list<F> list) : data(list) {}
	Vector(const std::vector<F> &data) : data(data) {}
	template <std::input_iterator Iter>
	Vector(Iter first, Iter last) : data(first, last) {}

	Vector(const Vector &) = default;
	Vector(Vector &&) noexcept = default;
	Vector &operator=(const Vector &) = default;
	Vector &operator=(Vector &&) noexcept = default;

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

	void clear() {
		assign(data.size(), zero);
	}

	void assign(size_t new_size, const F &value = zero) {
		data.assign(new_size, value);
	}

	F &operator[](size_t pos) noexcept {
		return data[pos];
	}

	const F &operator[](size_t pos) const noexcept {
		return data[pos];
	}

	size_t dim() const noexcept {
		return data.size();
	}

	void swap_values(size_t a, size_t b) noexcept(std::is_nothrow_swappable_v<F>) {
		using std::swap;
		swap(data[a], data[b]);
	}

	std::string repr() const {
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

	bool operator==(const Vector &other) const {
		return data == other.data;
	}

	Vector &operator+=(const Vector &other) {
		if (dim() != other.dim()) {
			throw std::runtime_error("Vector must be of the same dimension");
		}

		for (size_t i = 0; i < dim(); ++i) {
			data[i] += other[i];
		}

		return *this;
	}

	Vector operator+(const Vector &other) const {
		Vector result(*this);
		result += other;
		return result;
	}

	F operator*(const Vector &other) const {
		if (dim() != other.dim()) {
			throw std::runtime_error("Vector must be of the same dimension");
		}

		F sum = make_zero<F>();

		for (size_t i = 0; i < dim(); ++i) {
			sum += data[i] * other[i];
		}

		return sum;
	}

	Vector &operator*=(const F &scalar) {
		for (size_t i = 0; i < dim(); ++i) {
			data[i] *= scalar;
		}
		return *this;
	}
	Vector &operator/=(const F &scalar) {
		for (size_t i = 0; i < dim(); ++i) {
			data[i] /= scalar;
		}
		return *this;
	}

	Vector operator*(const F &scalar) const {
		Vector temp(*this);
		temp *= scalar;
		return temp;
	}
	friend Vector operator*(const F &scalar, const Vector &vec) {
		return vec * scalar;
	}

	Vector operator/(const F &scalar) const {
		Vector temp(*this);
		temp /= scalar;
		return temp;
	}
};


// ---- Predeclaration of sparse Matrices ----
template <class F>
class MatrixCSC;

/// @brief A dense matrix
template <class F>
class Matrix {
public:
	using value_type = F;

private:
	std::vector<std::vector<F>> data;
	size_t num_rows, num_cols;

	static constexpr F zero = make_zero<F>();

public:
	Matrix(const std::vector<std::vector<F>> &data) : data(data) {
		num_rows = data.size();
		if (num_rows != 0) {
			num_cols = data[0].size();
		}
		else {
			num_cols = 0;
		}
	}
	Matrix() noexcept : num_rows(0), num_cols(0) {}
	Matrix(size_t m, size_t n, const F &value) : data(m, std::vector<F>(n, value)), num_rows(m), num_cols(n) {}
	Matrix(size_t m, size_t n) : Matrix(m, n, zero) {}
	Matrix(std::initializer_list<std::initializer_list<F>> list) : num_cols(0) {
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
	Matrix(const MatrixCSC<F> &sparse);

	Matrix(const Matrix &) = default;
	Matrix(Matrix &&) noexcept = default;
	Matrix &operator=(const Matrix &) = default;
	Matrix &operator=(Matrix &&) noexcept = default;

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


	void clear() {
		assign(num_rows, num_cols, zero);
	}

	void assign(size_t m, size_t n, const F &value = zero) {
		num_rows = m;
		num_cols = n;
		data.assign(m, std::vector<F>(n, value));
	}

	F &operator()(size_t row, size_t col) noexcept {
		return data[row][col];
	}

	const F &operator()(size_t row, size_t col) const noexcept {
		return data[row][col];
	}

	std::vector<std::vector<F>> &rows() noexcept {
		return data;
	}

	const std::vector<std::vector<F>> &rows() const noexcept {
		return data;
	}

	size_t m() const noexcept {
		return num_rows;
	}

	size_t n() const noexcept {
		return num_cols;
	}

	std::string repr() const {
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

	void swap_rows(size_t a, size_t b) noexcept(std::is_nothrow_swappable_v<std::vector<F>>) {
		using std::swap;
		swap(data[a], data[b]);
	}

	bool is_square() const noexcept {
		return num_rows == num_cols;
	}

	bool operator==(const Matrix &other) const {
		return data == other.data;
	}

	Matrix &operator+=(const Matrix &other) {
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

	Matrix operator+(const Matrix &other) const {
		Matrix result(*this);
		result += other;
		return result;
	}

	Matrix operator*(const Matrix &other) const {
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

	Matrix &operator*=(const F &scalar) {
		for (size_t i = 0; i < num_rows; ++i) {
			for (size_t j = 0; j < num_cols; ++j) {
				(*this)(i, j) *= scalar;
			}
		}
		return *this;
	}
	Matrix &operator/=(const F &scalar) {
		for (size_t i = 0; i < num_rows; ++i) {
			for (size_t j = 0; j < num_cols; ++j) {
				(*this)(i, j) /= scalar;
			}
		}
		return *this;
	}

	Matrix operator*(const F &scalar) const {
		Matrix temp(*this);
		temp *= scalar;
		return temp;
	}

	friend Matrix operator*(const F &scalar, const Matrix &vec) {
		return vec * scalar;
	}

	Matrix operator/(const F &scalar) const {
		Matrix temp(*this);
		temp /= scalar;
		return temp;
	}
};


/// @brief A sparse matrix in the CSC format
template <class F>
class MatrixCSC {
public:
	using value_type = F;

private:
	std::vector<F> data;
	std::vector<size_t> rows;
	std::vector<size_t> ptrs;

	size_t num_rows;
	size_t num_cols;

	static constexpr F zero = make_zero<F>();

	// static ThreadPool &get_thread_pool();

public:
	MatrixCSC(size_t m, size_t n) noexcept : num_rows(m), num_cols(n) {}
	MatrixCSC() noexcept : MatrixCSC(0, 0) {}

	MatrixCSC(const Matrix<F> &matrix) : MatrixCSC(matrix.m(), matrix.n()) {
		ptrs.push_back(0);

		for (size_t j = 0; j < matrix.n(); ++j) {
			for (size_t i = 0; i < matrix.m(); ++i) {
				F value = matrix(i, j);
				if (is_zero(value)) continue;

				data.push_back(value);
				rows.push_back(i);
			}
			ptrs.push_back(data.size());
		}
	}

	MatrixCSC(const std::vector<std::vector<F>> &data) : MatrixCSC(Matrix<F>(data)) {}
	MatrixCSC(std::initializer_list<std::initializer_list<F>> list) : MatrixCSC(Matrix<F>(list)) {};

	MatrixCSC(size_t m, size_t n, const std::vector<F> &data, const std::vector<size_t> &rows, const std::vector<size_t> &ptrs) :
		MatrixCSC(m, n),
		data(data),
		rows(rows),
		ptrs(ptrs) {
	}

	MatrixCSC(size_t m, size_t n, std::vector<F> &&data, std::vector<size_t> &&rows, std::vector<size_t> &&ptrs) :
		MatrixCSC(m, n),
		data(std::move(data)),
		rows(std::move(rows)),
		ptrs(std::move(ptrs)) {
	}



	MatrixCSC(const MatrixCSC &) = delete;
	MatrixCSC &operator=(const MatrixCSC &) = delete;

	MatrixCSC(MatrixCSC &&other) noexcept : MatrixCSC(other.m(), other.n()) {
		data = std::move(other.data);
		rows = std::move(other.rows);
		ptrs = std::move(other.ptrs);

		other.num_rows = 0;
		other.num_cols = 0;
	}
	MatrixCSC &operator=(MatrixCSC &&other) noexcept {
		data = std::move(other.data);
		rows = std::move(other.rows);
		ptrs = std::move(other.ptrs);

		num_rows = other.num_rows;
		other.num_rows = 0;
		num_cols = other.num_cols;
		other.num_cols = 0;

		return *this;
	}

	~MatrixCSC() = default;

	// ---- sizes ----

	size_t m() const noexcept { return num_rows; }
	size_t n() const noexcept { return num_cols; }
	size_t nnz() const noexcept { return data.size(); }

	// ---- value getters ----

	size_t col_begin(size_t col) const noexcept { return ptrs[col]; }
	size_t col_end(size_t col) const noexcept { return ptrs[col]; }
	size_t row(size_t index) const noexcept { return rows[index]; }
	F elem(size_t index) const noexcept { return data[index]; }
};

// ---- Desparsification ----

template <class F>
Matrix<F>::Matrix(const MatrixCSC<F> &sparse) : Matrix(sparse.m(), sparse.n(), zero) {
	for (size_t j = 0; j < sparse.n(); ++j) {
		for (size_t i = sparse.col_begin(j); i < sparse.col_end(j); ++i) {
			size_t row = sparse.row(i);
			data[row][j] = sparse.elem(i);
		}
	}
}

// ---- Vector Matrix Operations ----

template <class F>
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

template <class F>
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


// ==== Solving Algorithms ====

class singular_matrix_exception : public std::runtime_error {
public:
	explicit singular_matrix_exception() : runtime_error("The matrix is singular!") {}
};

// ---- Sparse LU decomposition ----

/// @brief Decomposes the sparse matrix into two sparse L,U matrices
/// 
/// Uses the GE/LU algorithm, and Markowitz Pivoting
/// Taken from the book: Circuit Simulation (Farid N. Najm) 
/// 
/// @tparam F The field
/// @param matrix The matrix to be decomposed
/// @return Tuple of L, U respectively
template <class F>
std::tuple<MatrixCSC<F>, MatrixCSC<F>> LU_decompose(MatrixCSC<F> &matrix) {

}


// ---- GE on dense matrices ----
/* Solves the system Ax = b using the gaussian elimination */
template <class F>
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
