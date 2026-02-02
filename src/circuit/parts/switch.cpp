#include "circuit/parts/switch.h"


Switch::Switch(const std::string &name, bool on) : NPinPart<2>(name), branch_id(0), last_i(0.0), on(on) {}

std::vector<std::tuple<size_t, size_t, scalar>> Switch::gen_matrix_entries([[maybe_unused]] const StampParams &params) {
	const scalar req = on ? 0 : off_resistance;

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

void Switch::update(const StampParams &params) {
	bool new_on = on;

	while (!events.empty() && events.top().step <= params.step) {
		new_on = events.top().type == EventType::ON;
		events.pop();
	}

	on = new_on;
}

scalar Switch::get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const {
	return last_i;
}

void Switch::schedule_on(size_t step) {
	events.push({ .step = step, .type = EventType::ON });
}

void Switch::schedule_off(size_t step) {
	events.push({ .step = step, .type = EventType::OFF });
}
