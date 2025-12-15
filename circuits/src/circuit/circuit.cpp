#include "circuit.h"
#include "util.h"


Circuit::Circuit(scalar timestep, const fs::path& scope_export_path) :
	timestep(timestep),
	matrix(),
	scope_export_path(scope_export_path / make_timestamp())
{
	fs::create_directories(this->scope_export_path);
	ground = add_part<VoltageSource>("GND", 0.0f);
}


VoltageSource* Circuit::get_ground() const {
	return ground;
}

Node* Circuit::create_new_node() {
	auto node = std::make_unique<Node>();
	Node* raw = node.get();
	nodes.push_back(std::move(node));
	return raw;
}

void Circuit::connect(const Pin& pin_a, const Pin& pin_b) {
	if (pin_a.node == pin_b.node) {
		if (pin_a.node == nullptr) {
			Node* node = create_new_node();
			pin_a.owner->set_node(pin_a.pin_id, node);
			pin_b.owner->set_node(pin_b.pin_id, node);
		}
	}
	else if (pin_a.node == nullptr) {
		pin_a.owner->set_node(pin_a.pin_id, pin_b.node);
	}
	else {
		pin_b.owner->set_node(pin_b.pin_id, pin_a.node);
	}
}

void Circuit::update_parts() {
	StampParams params{
	.ground = ground->pin(),
	.timestep = timestep,
	.timestep_inv = 1.0 / timestep
	};

	// prepare the matrix and count the number of rows needed
	matrix.reset_row_count();
	for (auto& node : nodes) {
		node->node_id = matrix.reserve_row();
	}
	for (auto& part : parts) {
		part->pre_stamp(matrix, params);
	}

	matrix.init();

	// fill the matrix
	for (const auto& part : parts) {
		part->stamp(matrix, params);
	}

	matrix.solve();

	for (auto& node : nodes) {
		node->voltage = matrix.get_solution_value(node->node_id);
	}

	for (auto& part : parts) {
		part->post_stamp(matrix, params);
	}
}

void Circuit::run_for_steps(size_t num_steps) {
	scalar t = 0;

	for (size_t frame = 0; frame < num_steps; ++frame) {
		update_parts();

		for (const auto& scope : voltage_scopes) {
			scope->record_voltage(t);
		}
		for (const auto& scope : current_scopes) {
			scope->record_current(t);
		}

		t += timestep;
	}
}

void Circuit::run_for_seconds(scalar secs) {
	run_for_steps(static_cast<size_t>(secs/timestep));
}

// scopes
void Circuit::scope_voltage(const ConstPin& a, const ConstPin& b) {
	voltage_scopes.push_back(std::make_unique<VoltageScope>(a, b, scope_export_path));
}

void Circuit::scope_current(const ConstPin& a, const ConstPin& b) {
	current_scopes.push_back(std::make_unique<CurrentScope>(a, b, scope_export_path));
}

void Circuit::export_tables() const {
	for (const auto& scope : voltage_scopes) {
		scope->export_data();
	}
	for (const auto& scope : current_scopes) {
		scope->export_data();
	}
}
