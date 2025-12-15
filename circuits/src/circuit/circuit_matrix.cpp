#include "./circuit_matrix.h"


CircuitMatrix::CircuitMatrix() : matrix(), rhs(), num_rows(0), solution() {}

void CircuitMatrix::reset_row_count() {
	num_rows = 0;
}

size_t CircuitMatrix::reserve_row() {
	return num_rows++;
}

void CircuitMatrix::init() {
	matrix.assign(num_rows, num_rows);
	rhs.assign(num_rows);
}

void CircuitMatrix::stamp_template_I_out_LHS(const ConstPin& a, const ConstPin& b, size_t branch_id, scalar g_like_value, scalar cross_value) {
	matrix(branch_id, branch_id) += cross_value;

	if (!a.node->is_ground) {
		const size_t id = a.node->node_id;
		matrix(branch_id, id) += g_like_value;
		matrix(id, branch_id) += 1.0;
	}
	if (!b.node->is_ground) {
		const size_t id = b.node->node_id;
		matrix(branch_id, id) -= g_like_value;
		matrix(id, branch_id) -= 1.0;
	}
}

void CircuitMatrix::stamp_template_LHS(const ConstPin& a, const ConstPin& b, scalar value) {
	const bool a_ground = a.node->is_ground;
	const bool b_ground = b.node->is_ground;

	if (!a_ground && !b_ground) {
		const size_t row_1 = a.node->node_id;
		const size_t row_2 = b.node->node_id;
		matrix(row_1, row_1) += value;
		matrix(row_1, row_2) -= value;
		matrix(row_2, row_1) -= value;
		matrix(row_2, row_2) += value;
	}
	else if (a_ground && !b_ground) {
		const size_t row = b.node->node_id;
		matrix(row, row) += value;
	}
	else if (!a_ground && b_ground) {
		const size_t row = a.node->node_id;
		matrix(row, row) += value;
	}
}

void CircuitMatrix::stamp_template_RHS(size_t branch_id, scalar value) {
	rhs[branch_id] += value;
}

void CircuitMatrix::stamp_template_RHS(const ConstPin& a, const ConstPin& b, scalar value) {
	if (!a.node->is_ground) stamp_template_RHS(a.node->node_id, -value);
	if (!b.node->is_ground) stamp_template_RHS(b.node->node_id, value);
}


void CircuitMatrix::solve() {
	lingebra::solve_gaussian_elimination(matrix, rhs);
	solution = rhs;
}