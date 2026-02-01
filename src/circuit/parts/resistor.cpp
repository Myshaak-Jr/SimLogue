#include "circuit/parts/resistor.h"


Resistor::Resistor(const std::string &name, scalar ohms) : NPinPart<2>(name), ohms(ohms) {
	conductance = 1.0f / ohms;
}

std::vector<std::tuple<size_t, size_t, scalar>> Resistor::gen_matrix_entries([[maybe_unused]] const StampParams &params) {
	const auto &node0 = pin(0).node;
	const auto &node1 = pin(1).node;

	std::vector<std::tuple<size_t, size_t, scalar>> entries;

	if (!node0->is_ground && !node1->is_ground) {
		entries.push_back({ node0->node_id, node0->node_id, conductance });
		entries.push_back({ node0->node_id, node1->node_id, -conductance });
		entries.push_back({ node1->node_id, node0->node_id, -conductance });
		entries.push_back({ node1->node_id, node1->node_id, conductance });
	}
	else if (!node0->is_ground) {
		entries.push_back({ node0->node_id, node0->node_id, conductance });
	}
	else if (!node1->is_ground) {
		entries.push_back({ node1->node_id, node1->node_id, conductance });
	}

	return entries;
}

scalar Resistor::get_current_between(const ConstPin &a, const ConstPin &b) const {
	if (a.owner != this || b.owner != this) {
		throw std::runtime_error("Pins a and b must belong to this part.");
	}
	return conductance * (a.node->voltage - b.node->voltage);
}