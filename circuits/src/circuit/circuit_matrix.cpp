#include "./circuit_matrix.h"


CircuitMatrix::CircuitMatrix() {
	clear();
}

void CircuitMatrix::clear() {
	num_rows = 0;
	rhs.clear();
	matrix.clear();
}

size_t CircuitMatrix::reserve_row() {
	return num_rows++;
}

void CircuitMatrix::prepare_matrix() {
	matrix.assign(num_rows, std::vector<double>(num_rows, 0.0f));
	rhs.assign(num_rows, 0.0f);
}

void CircuitMatrix::stamp_conductance(size_t row_1, size_t row_2, double conductance) {
	matrix[row_1][row_1] = conductance;
	matrix[row_1][row_2] = -conductance;
	matrix[row_2][row_1] = -conductance;
	matrix[row_2][row_2] = conductance;
}

void CircuitMatrix::stamp_rhs(size_t row, double value) {

}