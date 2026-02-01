#include "circuit/parts/ac_voltage_source.h"

#include "circuit/util.h"

#include <cassert>
#include <cmath>
#include <numbers>



AcVoltageSource::AcVoltageSource(const std::string &name, scalar frequency, scalar amplitude, scalar phase) :
	NPinPart<1>(name),
	branch_id(0),
	current(0),
	amplitude(amplitude),
	phase(phase) {

	angular_vel = tau * frequency;
	voltage = amplitude * std::sin(phase);
}

AcVoltageSource::~AcVoltageSource() {}

std::vector<std::tuple<size_t, size_t, scalar>> AcVoltageSource::gen_matrix_entries([[maybe_unused]] const StampParams &params) {
	const auto &node = pin().node;
	if (node->is_ground) return {};

	return { {node->node_id, branch_id, 1.0}, {branch_id, node->node_id, 1.0} };
}

scalar AcVoltageSource::get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const {
	return current;
}

void AcVoltageSource::stamp_rhs_entries(std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) {
	rhs[branch_id] += voltage;
}

void AcVoltageSource::update_value_from_result([[maybe_unused]] size_t i, scalar value) {
	current = value;
}

void AcVoltageSource::update(const StampParams &params) {
	voltage = amplitude * std::sin(angular_vel * params.time + phase);
}


AcVoltageSource2Pin::AcVoltageSource2Pin(const std::string &name, scalar frequency, scalar amplitude, scalar phase) :
	NPinPart<2>(name),
	branch_id(0),
	current(0),
	amplitude(amplitude),
	phase(phase) {

	angular_vel = tau * frequency;
	voltage = amplitude * std::sin(phase);
}

AcVoltageSource2Pin::~AcVoltageSource2Pin() {}

std::vector<std::tuple<size_t, size_t, scalar>> AcVoltageSource2Pin::gen_matrix_entries([[maybe_unused]] const StampParams &params) {
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

void AcVoltageSource2Pin::stamp_rhs_entries(std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) {
	rhs[branch_id] += voltage;
}

scalar AcVoltageSource2Pin::get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const {
	return current;
}

void AcVoltageSource2Pin::update_value_from_result([[maybe_unused]] size_t i, scalar value) {
	current = value;
}

void AcVoltageSource2Pin::update(const StampParams &params) {
	voltage = amplitude * std::sin(angular_vel * params.time + phase);
}