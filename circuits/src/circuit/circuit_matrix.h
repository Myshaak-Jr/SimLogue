#pragma once

#include <vector>

#include "../lingebra.h"
#include "pin.h"
#include "scalar.h"


class CircuitMatrix {
private:
	using matrix_t = lingebra::Matrix<scalar>;
	using vector_t = lingebra::Vector<scalar>;

	matrix_t matrix;
	vector_t rhs;

	vector_t solution;

	size_t num_rows;

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


	//void stamp_conductance(const ConstPin& a, const ConstPin& b, scalar conductance);
	//inline void stamp_output_conductance(const ConstPin& a, const ConstPin& b, size_t branch_id, scalar conductance) {
	//	stamp_output_template(a, b, branch_id, conductance, -1.0, 0.0);
	//}

	//void stamp_current(const ConstPin& a, const ConstPin& b, scalar current);
	//inline void stamp_output_current(const ConstPin& a, const ConstPin& b, size_t branch_id, scalar current) {
	//	stamp_output_template(a, b, branch_id, 0.0, 1.0, current);
	//}

	//inline void stamp_capacitance(const ConstPin& a, const ConstPin& b, scalar capacitance, scalar prev_voltage) {
	//	const scalar g = capacitance * timestep_inv;
	//	stamp_conductance(a, b, g);
	//	stamp_current(a, b, -g * prev_voltage);
	//}
	//inline void stamp_output_capacitance(const ConstPin& a, const ConstPin& b, size_t branch_id, scalar capacitance, scalar prev_voltage) {
	//	const scalar g = capacitance * timestep_inv;
	//	stamp_output_template(a, b, branch_id, g, -1.0, g * prev_voltage);
	//}

	//inline void stamp_output_inductance(const ConstPin& a, const ConstPin& b, size_t branch_id, scalar inductance, scalar prev_current) {
	//	const scalar temp = inductance * timestep_inv;
	//	stamp_output_template(a, b, branch_id, 1.0, -temp, -temp * prev_current);
	//}

	//inline void stamp_output_voltage(const ConstPin& a, const ConstPin& b, size_t branch_id, scalar voltage) {
	//	stamp_output_template(a, b, branch_id, 1.0, 0.0, voltage);
	//}

	//void stamp_output_voltage(const ConstPin& p, size_t branch_id, scalar voltage);

	void solve();
	inline scalar get_solution_value(size_t id) const { return solution[id]; }
};

