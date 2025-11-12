#include "pch.h"
#include "CppUnitTest.h"

#include "string_repr.h"

#include <random>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace lingebra;

namespace Test {
	TEST_CLASS(TestModInt) {
	public:
		TEST_METHOD(TestInverse) {
			{
				Z_7 a = 3;
				auto b = a.inverse();
				Assert::AreEqual(Z_7(5), b);
			}
			{
				Z_5 a = 2;
				auto b = a.inverse();
				Assert::AreEqual(Z_5(3), b);
			}
			{
				Z_7 a = 6;
				auto b = a.inverse();
				Assert::AreEqual(Z_7(6), b);
			}
		}

		TEST_METHOD(TestArithmeticWithInts) {
			Z_7 a = 3;
			Z_7 b = 4;

			Z_7 c = a / b;

			a += 5;

			b -= 5;

			Assert::AreEqual(Z_7(1), a);
			Assert::AreEqual(Z_7(6), b);
			Assert::AreEqual(Z_7(6), c);
		}
	};

	TEST_CLASS(TestMatrix) {
		TEST_METHOD(TestVectorVectorProduct) {
			const Vector<Z_7> a = {2, 3, 4};
			const Vector<Z_7> b = {1, 2, 3};

			const Z_7 c = a * b;

			Assert::AreEqual(Z_7(6), c);
		}

		TEST_METHOD(TestMatrixVectorProduct) {
			const Matrix<Z_7> M = {
				{1, 2, 3},
				{4, 5, 6}
			};
			const Vector<Z_7> v = {1, 2, 3};

			const Vector<Z_7> r = M * v;

			const Vector<Z_7> expected = {0, 4};

			Assert::AreEqual(expected, r);
		}

		TEST_METHOD(TestVectorMatrixProduct) {
			const Vector<Z_7> v = {1, 2, 3};
			const Matrix<Z_7> M = {
				{1, 4},
				{2, 5},
				{3, 6}
			};

			const Vector<Z_7> r = v * M;

			const Vector<Z_7> expected = {0, 4};

			Assert::AreEqual(expected, r);
		}

		TEST_METHOD(TestMatrixMatrixProduct) {
			const Matrix<Z_7> A = {
				{1, 2, 3},
				{4, 5, 6}
			};
			const Matrix<Z_7> B = {
				{1, 4},
				{2, 5},
				{3, 6}
			};

			const Matrix<Z_7> C = A * B;

			const Matrix<Z_7> expected = {
				{0, 4},
				{4, 0}
			};

			Assert::AreEqual(expected, C);
		}

		TEST_METHOD(TestGaussianElimination) {
			std::mt19937 rng(0);

			constexpr size_t num_tests = 300;

			for (size_t i = 0; i < num_tests; ++i) {
				size_t n = 2 + i / 3;
				const Matrix<Z_7> M = Matrix<Z_7>::make_random(rng, n, n);
				const Vector<Z_7> b = Vector<Z_7>::make_random(rng, n);

				Matrix<Z_7> A = M;
				Vector<Z_7> x = b;
				
				try {
					solve_gaussian_elimination(A, x);
				}
				catch (const singular_matrix_exception& e) {
					// maybe double check for this with determinants
					continue;
				}

				const Vector<Z_7> test_b = M * x;

				Assert::AreEqual(b, test_b);
			}
		}
	};
}
