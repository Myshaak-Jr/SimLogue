#include "../circuit_matrix.h"
#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include "inductor.h"
#include <string>


Inductor::Inductor(const std::string &name, scalar inductance) :
	NPinPart<2>(name),
	inductance(inductance),
	branch_id(0),
	last_i(0.0) {
}

void Inductor::pre_stamp(CircuitMatrix &matrix, const StampParams &params) {
	branch_id = matrix.reserve_row();
}

void Inductor::stamp(CircuitMatrix &matrix, const StampParams &params) const {
	const scalar req = inductance * params.timestep_inv;

	matrix.stamp_template_I_out_LHS(pin(0), pin(1), branch_id, 1.0, -req);
	matrix.stamp_template_RHS(branch_id, -req * last_i);
}

void Inductor::post_stamp(const CircuitMatrix &matrix, const StampParams &params) {
	last_i = matrix.get_solution_value(branch_id);
}

std::vector<std::tuple<size_t, size_t, scalar>> Inductor::gen_matrix_entries(scalar timestep) const {
	const scalar req = inductance * (1.0 / timestep);

	std::vector<std::tuple<size_t, size_t, scalar>> entries;

	entries.push_back({ branch_id, branch_id, -req });

	const auto &node0 = pin(0).node;
	const auto &node1 = pin(1).node;

	if (!node0->is_ground) {
		entries.push_back({ node0->node_id, branch_id, 1.0 });
		entries.push_back({ branch_id, node0->node_id, 1.0 });
	}
	if (!node1->is_ground) {
		entries.push_back({ node1->node_id, branch_id, -1.0 });
		entries.push_back({ branch_id, node1->node_id, -1.0 });
	}

	return entries;
}

scalar Inductor::get_current_between(const ConstPin &a, const ConstPin &b) const {
	return last_i;
}
