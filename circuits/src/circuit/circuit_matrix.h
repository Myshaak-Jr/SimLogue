#pragma once

#include <vector>

#include "../lingebra.h"
#include "pin.h"
#include "scalar.h"


class CircuitMatrix {
private:
	using matrix_t = lingebra::MatrixCSC<scalar>;
	using vector_t = lingebra::Vector<scalar>;

	matrix_t matrix;
	vector_t rhs;

	vector_t solution;

	size_t num_rows;

	struct Entry {
		size_t i;
		size_t j;
		scalar v;
	};

	std::vector<Entry> entries;

public:
	CircuitMatrix();

	void reset_row_count();
	size_t reserve_row();

	// resize & clear
	void init();

	// stamps, the current always flows a -> b

	void stamp_template_I_out_LHS(const ConstPin &a, const ConstPin &b, size_t branch_id, scalar g_like_value, scalar cross_value);
	void stamp_template_LHS(const ConstPin &a, const ConstPin &b, scalar value);
	void stamp_template_RHS(size_t branch_id, scalar value);
	void stamp_template_RHS(const ConstPin &a, const ConstPin &b, scalar value);

	void solve();
	scalar get_solution_value(size_t id) const { return solution[id]; }
};

