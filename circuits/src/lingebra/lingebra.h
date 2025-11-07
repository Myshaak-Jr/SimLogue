#pragma once

#include <vector>
#include <stdexcept>
#include <limits>
#include <cmath>


namespace lingebra {
	constexpr bool is_prime(size_t n) noexcept {
		if (n < 2) return false;
		for (size_t i = 2; i * i <= n; ++i) {
			if (n % i == 0) return false;
		}
		return true;
	}

	template <size_t N>
	class ModInt {
	public:
		using data_type =
			std::conditional_t<(N <= 127), int8_t,
			std::conditional_t<(N <= 32767), int16_t,
			std::conditional_t<(N <= 2147483647), int32_t, int64_t>>>;

	private:
		data_type value;
		static constexpr data_type n = static_cast<data_type>(N);

	public:
		constexpr ModInt(data_type value = 0) noexcept : value(((value% n) + n) % n) {}

		constexpr data_type get() const noexcept { return value; }
		constexpr data_type get_n() const noexcept { return n; }

		/* uses Extended Euclidian Algorithm */
		constexpr ModInt inverse() const noexcept requires (is_prime(N)) {
			data_type t = 0;
			data_type new_t = 1;
			data_type r = n;
			data_type new_r = value;

			while (new_r != 0) {
				const data_type q = r / new_r;
				const data_type temp_r = r - q * new_r;
				r = new_r;
				new_r = temp_r;
				const data_type temp_t = t - q * new_t;
				t = new_t;
				new_t = temp_t;
			}

			return ModInt(t);
		}

		friend auto operator<=>(const ModInt&, const ModInt&) = default;

		constexpr ModInt& operator+=(const ModInt& other) noexcept {
			value += other.value;
			if (value >= n) value -= n;
			return *this;
		}

		constexpr ModInt& operator+=(const ModInt& other) noexcept {
			value -= other.value;
			if (value < 0) value += n;
			return *this;
		}

		constexpr ModInt& operator*=(const ModInt& other) noexcept {
			uint64_t temp = static_cast<uint64_t>(value) * other.value;
			value = temp % n;
		}

		constexpr ModInt& operator/=(const ModInt& other) noexcept requires (is_prime(N)) {



			return *this;
		}


		ModInt operator+(const ModInt& other) {
			return ModInt(value + other.value);
		}
		ModInt operator-(const ModInt& other) {
			return ModInt(value - other.value);
		}
	};


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
			return T{0};
		}
	}

	template <has_zero T>
	constexpr bool is_zero(const T& a) {
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
			return T{1};
		}
	}

	template <class T>
	concept additive =
		requires (T a, T b) { { a + b } -> std::convertible_to<T>; }&&
		requires (T a, T b) { { b + a } -> std::convertible_to<T>; }&&
		requires (T a, T b) { { a - b } -> std::convertible_to<T>; }&&
		requires (T a, T b) { { b - a } -> std::convertible_to<T>; };

	template <class T>
	concept multiplicative =
		requires (T a, T b) { { a* b } -> std::convertible_to<T>; }&&
		requires (T a, T b) { { b* a } -> std::convertible_to<T>; }&&
		requires (T a, T b) { { a / b } -> std::convertible_to<T>; }&&
		requires (T a, T b) { { b / a } -> std::convertible_to<T>; };

	template <class T>
	concept division_ring = additive<T> && multiplicative<T> &&
		requires { make_one<T>() != make_zero<T>(); };



	template <division_ring TData>
	class Vector {
	private:
		std::vector<TData> data;

		static constexpr TData zero = make_zero<TData>();

	public:
		Vector() {}
		Vector(const size_t size, const TData& value) : data(size, value) {}
		explicit Vector(const size_t size) : Vector(size, zero) {}
		Vector(std::initializer_list<TData> list) : data(list) {}
		Vector(const std::vector<TData>& data) : data(data) {}

		~Vector() = default;

		void clear() {
			assign(data.size(), zero);
		}

		void assign(const size_t new_size) {
			assign(new_size, zero);
		}
		void assign(const size_t new_size, const TData& value) {
			data.assign(new_size, value);
		}

		TData& operator[](const size_t pos) {
			return data[pos];
		}

		const TData& operator[](const size_t pos) const {
			return data[pos];
		}

		size_t dim() const {
			return data.size();
		}

		void swap_values(const size_t a, const size_t b) {
			std::swap(data[a], data[b]);
		}
	};


	//template <division_ring TData>
	//class SparseMatrix;

	template <division_ring TData>
	class Matrix {
	private:
		std::vector<std::vector<TData>> data;
		size_t num_rows, num_cols;

		static constexpr TData zero = make_zero<TData>();

	public:
		Matrix(const std::vector<std::vector<TData>>& data) : data(data) {
			num_rows = data.size();
			if (num_rows != 0) {
				num_cols = data[0].size();
			}
			else {
				num_cols = 0;
			}
		}
		Matrix() : num_rows(0), num_cols(0) {}
		Matrix(const size_t m, const size_t n, const TData& value) : data(m, std::vector<TData>(n, value)), num_rows(m), num_cols(n) {}
		Matrix(const size_t m, const size_t n) : Matrix(m, n, zero) {}
		Matrix(std::initializer_list<std::initializer_list<TData>> list) {
			num_rows = list.size();
			bool num_cols_set = false;
			for (const std::initializer_list<TData>& row : list) {
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
		//Matrix(const SparseMatrix<TData>& sparse_matrix);

		~Matrix() = default;

		void clear() {
			assign(num_rows, num_cols, zero);
		}

		void assign(const size_t m, const size_t n) {
			assign(m, n, zero);
		}

		void assign(const size_t m, const size_t n, const TData& value) {
			data.assign(m, std::vector<TData>(n, value));
		}

		TData& operator()(const size_t row, const size_t col) {
			return data[row][col];
		}

		const TData& operator()(const size_t row, const size_t col) const {
			return data[row][col];
		}

		std::vector<std::vector<TData>>& rows() {
			return data;
		}

		const std::vector<std::vector<TData>>& rows() const {
			return data;
		}

		size_t m() const {
			return num_rows;
		}

		size_t n() const {
			return num_cols;
		}

		void swap_rows(const size_t a, const size_t b) {
			std::swap(data[a], data[b]);
		}

		bool is_square() const {
			return num_rows == num_cols;
		}
	};


	// A sparse matrix implementation
	//template <number TData>
	//class SparseMatrix {
	//private:
	//	size_t num_rows, num_cols;
	//	
	//	std::vector<TData> values;
	//	std::vector<size_t> col_index;
	//	std::vector<size_t> row_index;


	//public:
	//	SparseMatrix(const Matrix<TData>& matrix) : num_rows(matrix.m()), num_cols(matrix.n()) {
	//		for (const std::vector<TData> row : matrix.rows()) {
	//			for (TData value : row) {
	//				if ()
	//			}
	//		}
	//	}

	//	SparseMatrix(const std::vector<std::vector<TData>>& data) : SparseMatrix(Matrix<TData>(data)) {}
	//	SparseMatrix(std::initializer_list<std::initializer_list<TData>> list) : SparseMatrix(Matrix<TData>(list)) {}

	//	SparseMatrix() : num_rows(0), num_cols(0) {}
	//	SparseMatrix(const size_t m, const size_t n) : num_rows(m), num_cols(n) {}

	//	TData& operator()(const size_t row, const size_t col) {

	//	}

	//

	//	size_t num_nonzeros() const {
	//		return values.size();
	//	}

	//	size_t m() const {
	//		return num_rows;
	//	}

	//	size_t n() const {
	//		return num_cols;
	//	}
	//};

	template <division_ring TData>
	auto operator*(const Vector<TData>& vec1, const Vector<TData>& vec2) {
		if (vec1.dim() != vec2.dim()) {
			throw std::runtime_error("Vector must be of the same dimension");
		}

		TData sum = make_zero<TData>();

		for (size_t i = 0; i < vec1.dim(); ++i) {
			sum += vec1[i] * vec2[i];
		}

		return sum;
	}

	template <division_ring TData>
	Vector<TData> operator*(const Matrix<TData>& matrix, const Vector<TData>& vector) {
		if (matrix.n() != vector.dim()) {
			throw std::runtime_error("Uncompatible matrix, vector size");
		}

		Vector<TData> result(matrix.m());

		for (size_t i = 0; i < matrix.m(); ++i) {
			TData sum = make_zero<TData>();
			for (size_t j = 0; j < matrix.n(); ++j) {
				sum += matrix(i, j) * vector[j];
			}
			result[i] = sum;
		}

		return result;
	}


	class singular_matrix_exception : public std::runtime_error {
	public:
		explicit singular_matrix_exception() : runtime_error("The matrix is singular!") {}
	};


	/* Solves the system Ax = b using the gassian elimination, the result is then stored in the vector b, it modifies the matrix so make a copy if you want to keep it */
	template <division_ring T>
	void solve_gaussian_elimination(Matrix<T>& matrix, Vector<T>& b) {
		if (!matrix.is_square()) {
			throw singular_matrix_exception();
		}

		const size_t order = matrix.n();

		// ensure that there is no zero on the diagonal
		for (size_t i = 0; i < order; ++i) {
			if (!is_zero(matrix(i, i))) continue;
			bool swapped = false;
			for (size_t j = 0; j < order; ++j) {
				if (i == j) continue;
				if (is_zero(matrix(i, j))) continue;
				matrix.swap_rows(i, j);
			}
			if (!swapped) throw singular_matrix_exception();
		}

		for (size_t i = 0; i < order; ++i) {
			const T factor = make_one<T>() / matrix(i, i);
			for (size_t j = 0; j < order; ++j) {
				if (i == j) continue;
				matrix(i, j) *= factor;
			}
			b[i] *= factor;

			for (size_t j = 0; j < order; ++j) {
				if (i == j) continue;
				const T factor = make_one<T>() / matrix(j, i);

				if (is_zero(matrix(j, i))) continue;

				for (size_t k = i + 1; k < order; ++k) {
					matrix(j, k) *= factor;
					matrix(j, k) -= matrix(i, k);
				}
				b[j] *= factor;
				b[j] -= b[i];
			}
		}
	}
}