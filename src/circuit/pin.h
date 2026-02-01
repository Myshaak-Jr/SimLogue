#pragma once

#include "circuit/node.h"

#include <string>


class Part;

struct Pin {
	size_t pin_id;
	Node *node;
	Part *owner;
	std::string name;

	constexpr Pin(size_t pin_id, Node *node, Part *owner, const std::string &name) : pin_id(pin_id), node(node), owner(owner), name(name) {}
};

struct ConstPin {
	size_t pin_id;
	const Node *node;
	const Part *owner;
	std::string name;

	constexpr ConstPin(size_t pin_id, const Node *node, const Part *owner, const std::string &name) : pin_id(pin_id), node(node), owner(owner), name(name) {}
	constexpr ConstPin(const Pin &pin) : pin_id(pin.pin_id), node(pin.node), owner(pin.owner), name(pin.name) {}
};
