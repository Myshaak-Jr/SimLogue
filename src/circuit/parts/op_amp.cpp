#include "circuit/parts/op_amp.h"

#include "circuit/util.h"


OpAmp::OpAmp(const std::string &name, scalar v_min, scalar v_max, scalar amplification) :
	NPinPart<3>(name),
	v_min(v_min),
	v_max(v_max),
	amplification(amplification),
	branch_id(0),
	mode(Mode::Linear) {
}

std::vector<MatrixEntry> OpAmp::gen_matrix_entries([[maybe_unused]] const StampParams &params) {
	const auto &node = pin(Pins::Out).node;
	if (node->is_ground) return {};

	const auto &node_plus = pin(Pins::Plus).node;
	const auto &node_minus = pin(Pins::Minus).node;

	std::vector<MatrixEntry> entries;

	entries.push_back({ node->node_id, branch_id, 1.0 });
	entries.push_back({ branch_id, node->node_id, 1.0 });

	switch (mode) {
		case Mode::Linear:
			if (!node_plus->is_ground) {
				entries.push_back({ branch_id, node_plus->node_id, -amplification });
			}
			if (!node_minus->is_ground) {
				entries.push_back({ branch_id, node_minus->node_id, amplification });
			}
			break;

		default:
			break;
	}

	return entries;
}

void OpAmp::stamp_rhs_entries([[maybe_unused]] std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) {
	switch (mode) {
		case Mode::SatHigh:
			rhs[branch_id] += v_max;
			break;

		case Mode::SatLow:
			rhs[branch_id] += v_min;
			break;

		default:
			break;
	}
}

void OpAmp::update([[maybe_unused]] const StampParams &params) {
	const auto &node_plus = pin(Pins::Plus).node;
	const auto &node_minus = pin(Pins::Minus).node;

	scalar diff = amplification * (node_plus->voltage - node_minus->voltage);

	switch (mode) {
		case Mode::Linear:
			if (diff > v_max + hysteresis) mode = Mode::SatHigh;
			else if (diff < v_min - hysteresis) mode = Mode::SatLow;
			break;

		case Mode::SatHigh:
			if (diff < v_max - hysteresis) mode = Mode::Linear;
			break;

		case Mode::SatLow:
			if (diff > v_min + hysteresis) mode = Mode::Linear;
			break;
	}
}
