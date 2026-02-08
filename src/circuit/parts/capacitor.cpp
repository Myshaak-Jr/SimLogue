#include "circuit/parts/capacitor.h"

#include "circuit/n_pin_part.h"
#include "circuit/part.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <string>



Capacitor::Capacitor(const std::string &name, scalar capacitance) :
	NPinPart<2>(name),
	capacitance(capacitance),
	last_v(0.0),
	last_i(0.0),
	admittance(0.0) {
}


std::vector<MatrixEntry> Capacitor::gen_matrix_entries(const StampParams &params) {
	admittance = capacitance * params.timestep_inv;

	const auto &node0 = pin(0).node;
	const auto &node1 = pin(1).node;

	std::vector<MatrixEntry> entries;

	if (!node0->is_ground && !node1->is_ground) {
		entries.push_back({ node0->node_id, node0->node_id, admittance });
		entries.push_back({ node0->node_id, node1->node_id, -admittance });
		entries.push_back({ node1->node_id, node0->node_id, -admittance });
		entries.push_back({ node1->node_id, node1->node_id, admittance });
	}
	else if (!node0->is_ground) {
		entries.push_back({ node0->node_id, node0->node_id, admittance });
	}
	else if (!node1->is_ground) {
		entries.push_back({ node1->node_id, node1->node_id, admittance });
	}

	return entries;
}

void Capacitor::stamp_rhs_entries(std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) {
	const auto &node0 = pin(0).node;
	const auto &node1 = pin(1).node;

	auto value = admittance * last_v;

	if (!node0->is_ground) rhs[node0->node_id] += value;
	if (!node1->is_ground) rhs[node1->node_id] += -value;
}

void Capacitor::update([[maybe_unused]] const StampParams &params) {
	scalar v_now = pin(0).node->voltage - pin(1).node->voltage;
	last_i = admittance * (v_now - last_v);
	last_v = v_now;
}


scalar Capacitor::get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const {
	return last_i;
}