#include "circuit/parts/voltage_source.h"

#include <cassert>


VoltageSource::VoltageSource(const std::string &name, scalar voltage) : NPinPart<1>(name), voltage(voltage), branch_id(0), current(0) {}

VoltageSource::~VoltageSource() {}

std::vector<std::tuple<size_t, size_t, scalar>> VoltageSource::gen_matrix_entries([[maybe_unused]] const StampParams &params) {
	const auto &node = pin().node;
	if (node->is_ground) return {};

	return { {node->node_id, branch_id, 1.0}, {branch_id, node->node_id, 1.0} };
}

scalar VoltageSource::get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const {
	return current;
}

void VoltageSource::stamp_rhs_entries(std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) {
	rhs[branch_id] += voltage;
}

void VoltageSource::update_value_from_result([[maybe_unused]] size_t i, scalar value) {
	current = value;
}



VoltageSource2Pin::VoltageSource2Pin(const std::string &name, scalar voltage) : NPinPart<2>(name), voltage(voltage), branch_id(0), current(0) {}

VoltageSource2Pin::~VoltageSource2Pin() {}

std::vector<std::tuple<size_t, size_t, scalar>> VoltageSource2Pin::gen_matrix_entries([[maybe_unused]] const StampParams &params) {
	const auto &node0 = pin(0).node;
	const auto &node1 = pin(1).node;

	std::vector<std::tuple<size_t, size_t, scalar>> entries;

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

void VoltageSource2Pin::stamp_rhs_entries(std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) {
	rhs[branch_id] += voltage;
}

scalar VoltageSource2Pin::get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const {
	return current;
}

void VoltageSource2Pin::update_value_from_result([[maybe_unused]] size_t i, scalar value) {
	current = value;
}