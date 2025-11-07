#include "./circuit.h"


Circuit::Circuit() : matrix() {
	ground = add_part<VoltageSource>(0.0f);
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

void Circuit::connect(Pin pin_a, Pin pin_b) {
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

void Circuit::update() {
	// prepare the matrix
	matrix.clear();
	for (auto& node : nodes) {
		node->set_row_id(matrix.reserve_row());
	}

	for (auto& part : parts) {
		if (part->has_ammeter()) {
			Ammeter* am = part->get_ammeter();
			am->set_row_id(matrix.reserve_row());
		}
	}

	// propagate the matrix
	for (auto& part : parts) {
		part->stamp(matrix);
	}


}