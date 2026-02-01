#include "circuit/parts/inductor.h"

#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include <string>


Inductor::Inductor(const std::string &name, scalar inductance) :
	NPinPart<2>(name),
	inductance(inductance),
	last_i(0.0),
	branch_id(0) {
}

std::vector<std::tuple<size_t, size_t, scalar>> Inductor::gen_matrix_entries(const StampParams &params) {
	const scalar req = inductance * params.timestep_inv;

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

void Inductor::stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) {
	const scalar req = inductance * params.timestep_inv;
	rhs[branch_id] += -req * last_i;
}

scalar Inductor::get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const {
	return last_i;
}