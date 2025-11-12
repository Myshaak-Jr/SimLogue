#pragma once

#include <vector>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <iterator>
#include <random>


namespace lingebra {
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
		constexpr ModInt(value_type value = 0) noexcept : value(((value% n) + n) % n) {}

		template <class TEngine>
		static ModInt make_random(TEngine& engine) {
			std::uniform_int_distribution<uint64_t> dist(0, static_cast<uint16_t>(get_n()) - 1);
			return ModInt(static_cast<value_type>(dist(engine)));
		}

		constexpr value_type get() const noexcept { return value; }
		constexpr static value_type get_n() noexcept { return n; }

		friend auto operator<=>(const ModInt&, const ModInt&) = default;

		constexpr ModInt& operator+=(const ModInt& other) noexcept {
			value += other.value;
			if (value >= n) value -= n;
			return *this;
		}
		constexpr ModInt& operator-=(const ModInt& other) noexcept {
			value -= other.value;
			if (value < 0) value += n;
			return *this;
		}
		constexpr ModInt& operator*=(const ModInt& other) noexcept {
			value = (static_cast<uint64_t>(value) * other.value) % n;
			return *this;
		}

		constexpr ModInt& operator++() noexcept {
			if (++value == n) value = 0;
			return *this;
		}
		constexpr ModInt operator++(int) noexcept {
			ModInt temp = *this;
			++(*this);
			return temp;
		}

		constexpr ModInt operator+(const ModInt& other) const noexcept {
			ModInt temp = *this;
			temp += other;
			return temp;
		}
		constexpr ModInt operator-(const ModInt& other) const noexcept {
			ModInt temp = *this;
			temp -= other;
			return temp;
		}
		constexpr ModInt operator*(const ModInt& other) const noexcept {
			ModInt temp = *this;
			temp *= other;
			return temp;
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

		constexpr ModInt& operator/=(const ModInt& other) noexcept requires (is_prime(N)) {
			(*this) *= other.inverse();
			return *this;
		}

		constexpr ModInt operator/(const ModInt& other) noexcept requires (is_prime(N)) {
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
	concept field = additive<T> && multiplicative<T> &&
		requires { make_one<T>() != make_zero<T>(); };


	// Vectors and Matrices
	template <field TData>
	class Vector {
	public:
		using value_type = TData;

	private:
		std::vector<TData> data;

		static constexpr TData zero = make_zero<TData>();

	public:
		constexpr Vector() noexcept = default;
		constexpr Vector(const size_t size, const TData& value) : data(size, value) {}
		constexpr explicit Vector(const size_t size) : Vector(size, zero) {}
		constexpr Vector(std::initializer_list<TData> list) : data(list) {}
		constexpr Vector(const std::vector<TData>& data) : data(data) {}
		template <std::input_iterator Iter>
		constexpr Vector(Iter first, Iter last) : data(first, last) {}

		constexpr Vector(const Vector&) = default;
		constexpr Vector(Vector&&) noexcept = default;
		constexpr Vector& operator=(const Vector&) = default;
		constexpr Vector& operator=(Vector&&) noexcept = default;

		~Vector() = default;


		// random generation (only for finite division rings)
		template <class TEngine>
		static Vector make_random(TEngine& engine, const size_t size) requires ModIntLike<TData> {
			Vector vec(size);
			for (size_t i = 0; i < size; ++i) {
				vec[i] = TData::make_random(engine);
			}
			return vec;
		}


		constexpr void clear() {
			assign(data.size(), zero);
		}

		constexpr void assign(const size_t new_size) {
			assign(new_size, zero);
		}

		constexpr void assign(const size_t new_size, const TData& value) {
			data.assign(new_size, value);
		}

		constexpr TData& operator[](const size_t pos) noexcept {
			return data[pos];
		}

		constexpr const TData& operator[](const size_t pos) const noexcept {
			return data[pos];
		}

		constexpr size_t dim() const noexcept {
			return data.size();
		}

		constexpr void swap_values(const size_t a, const size_t b) noexcept(std::is_nothrow_swappable_v<TData>) {
			std::swap(data[a], data[b]);
		}

		constexpr bool operator==(const Vector& other) const {
			return data == other.data;
		}

		constexpr Vector& operator+=(const Vector& other) {
			if (dim() != other.dim()) {
				throw std::runtime_error("Vector must be of the same dimension");
			}

			for (size_t i = 0; i < dim(); ++i) {
				data[i] += other[i];
			}

			return *this;
		}

		constexpr Vector operator+(const Vector& other) const {
			Vector result(*this);
			result += other;
			return result;
		}

		constexpr TData operator*(const Vector& other) const {
			if (dim() != other.dim()) {
				throw std::runtime_error("Vector must be of the same dimension");
			}

			TData sum = make_zero<TData>();

			for (size_t i = 0; i < dim(); ++i) {
				sum += data[i] * other[i];
			}

			return sum;
		}

		constexpr Vector& operator*=(const TData& scalar) {
			for (size_t i = 0; i < dim(); ++i) {
				data[i] *= scalar;
			}
			return *this;
		}
		constexpr Vector& operator/=(const TData& scalar) {
			for (size_t i = 0; i < dim(); ++i) {
				data[i] /= scalar;
			}
			return *this;
		}

		constexpr Vector operator*(const TData& scalar) const {
			Vector temp(*this);
			temp *= scalar;
			return temp;
		}
		constexpr friend Vector operator*(const TData& scalar, const Vector& vec) {
			return vec * scalar;
		}

		constexpr Vector operator/(const TData& scalar) const {
			Vector temp(*this);
			temp /= scalar;
			return temp;
		}
	};


	// A dense matrix
	template <field TData>
	class Matrix {
	public:
		using value_type = TData;

	private:
		std::vector<std::vector<TData>> data;
		size_t num_rows, num_cols;

		static constexpr TData zero = make_zero<TData>();

	public:
		constexpr Matrix(const std::vector<std::vector<TData>>& data) : data(data) {
			num_rows = data.size();
			if (num_rows != 0) {
				num_cols = data[0].size();
			}
			else {
				num_cols = 0;
			}
		}
		constexpr Matrix() noexcept : num_rows(0), num_cols(0) {}
		constexpr Matrix(const size_t m, const size_t n, const TData& value) : data(m, std::vector<TData>(n, value)), num_rows(m), num_cols(n) {}
		constexpr Matrix(const size_t m, const size_t n) : Matrix(m, n, zero) {}
		constexpr Matrix(std::initializer_list<std::initializer_list<TData>> list) : num_cols(0) {
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

		constexpr Matrix(const Matrix&) = default;
		constexpr Matrix(Matrix&&) noexcept = default;
		constexpr Matrix& operator=(const Matrix&) = default;
		constexpr Matrix& operator=(Matrix&&) noexcept = default;

		~Matrix() = default;

		
		// random generation (only for finite division rings)
		template <class TEngine>
		static Matrix make_random(TEngine& engine, const size_t m, const size_t n) requires ModIntLike<TData> {
			Matrix mat(m, n);
			for (size_t i = 0; i < m; ++i) {
				for (size_t j = 0; j < n; ++j) {
					mat(i, j) = TData::make_random(engine);
				}
			}
			return mat;
		}


		constexpr void clear() {
			assign(num_rows, num_cols, zero);
		}

		constexpr void assign(const size_t m, const size_t n) {
			assign(m, n, zero);
		}

		constexpr void assign(const size_t m, const size_t n, const TData& value) {
			data.assign(m, std::vector<TData>(n, value));
		}

		constexpr TData& operator()(const size_t row, const size_t col) noexcept {
			return data[row][col];
		}

		constexpr const TData& operator()(const size_t row, const size_t col) const noexcept {
			return data[row][col];
		}

		constexpr std::vector<std::vector<TData>>& rows() noexcept {
			return data;
		}

		constexpr const std::vector<std::vector<TData>>& rows() const noexcept {
			return data;
		}

		constexpr size_t m() const noexcept {
			return num_rows;
		}

		constexpr size_t n() const noexcept {
			return num_cols;
		}

		constexpr void swap_rows(const size_t a, const size_t b) noexcept(std::is_nothrow_swappable_v<std::vector<TData>>) {
			std::swap(data[a], data[b]);
		}

		constexpr bool is_square() const noexcept {
			return num_rows == num_cols;
		}

		constexpr bool operator==(const Matrix& other) const {
			return data == other.data;
		}

		constexpr Matrix& operator+=(const Matrix& other) {
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

		constexpr Matrix operator+(const Matrix& other) const {
			Matrix result(*this);
			result += other;
			return result;
		}

		constexpr Matrix operator*(const Matrix& other) const {
			if (num_cols != other.num_rows) {
				throw std::runtime_error("Uncompatible matrices for matrix product");
			}

			Matrix result(num_rows, other.num_cols);

			for (size_t i = 0; i < num_rows; ++i) {
				for (size_t j = 0; j < other.num_cols; ++j) {
					TData sum = make_zero<TData>();

					for (size_t k = 0; k < num_cols; ++k) {
						sum += (*this)(i, k) * other(k, j);
					}

					result(i, j) = sum;
				}
			}

			return result;
		}

		constexpr Matrix& operator*=(const TData& scalar) {
			for (size_t i = 0; i < num_rows; ++i) {
				for (size_t j = 0; j < num_cols; ++j) {
					(*this)(i, j) *= scalar;
				}
			}
			return *this;
		}
		constexpr Matrix& operator/=(const TData& scalar) {
			for (size_t i = 0; i < num_rows; ++i) {
				for (size_t j = 0; j < num_cols; ++j) {
					(*this)(i, j) /= scalar;
				}
			}
			return *this;
		}

		constexpr Matrix operator*(const TData& scalar) const {
			Matrix temp(*this);
			temp *= scalar;
			return temp;
		}
		constexpr friend Matrix operator*(const TData& scalar, const Matrix& vec) {
			return vec * scalar;
		}

		constexpr Matrix operator/(const TData& scalar) const {
			Matrix temp(*this);
			temp /= scalar;
			return temp;
		}
	};


	template <field TData>
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

	template <field TData>
	Vector<TData> operator*(const Vector<TData>& vector, const Matrix<TData>& matrix) {
		if (matrix.m() != vector.dim()) {
			throw std::runtime_error("Uncompatible matrix, vector size");
		}

		Vector<TData> result(matrix.n());

		for (size_t i = 0; i < matrix.n(); ++i) {
			TData sum = make_zero<TData>();
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


	/* Solves the system Ax = b using the gassian elimination, the result is then stored in the vector b, it modifies the matrix so make a copy if you want to keep it */
	template <field T>
	void solve_gaussian_elimination(Matrix<T>& matrix, Vector<T>& b) {
		if (!matrix.is_square())
			throw singular_matrix_exception();
		const size_t n = matrix.n();
		if (b.dim() != n)
			throw std::runtime_error("Size mismatch in solve_gaussian_elimination");

		for (size_t i = 0; i < n; ++i) {
			// ensure that there is no zero on the diagonal
			if (is_zero(matrix(i, i))) {
				bool swapped = false;
				for (size_t j = i + 1; j < n; ++j) {
					if (is_zero(matrix(j, i))) continue;
					matrix.swap_rows(i, j);
					b.swap_values(i, j);
					swapped = true;
					break;
				}
				if (!swapped) throw singular_matrix_exception();
			}

			// normalize the row
			const T factor = make_one<T>() / matrix(i, i);
			for (size_t j = i + 1; j < n; ++j) { // we wont need the previous values anymore since we have nonzero diagonal
				matrix(i, j) *= factor;
			}
			b[i] *= factor;

			for (size_t j = 0; j < n; ++j) {
				if (i == j) continue;
				const T factor = matrix(j, i);

				if (is_zero(factor)) continue;

				for (size_t k = i + 1; k < n; ++k) {
					matrix(j, k) -= factor * matrix(i, k);
				}
				b[j] -= factor * b[i];
			}
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