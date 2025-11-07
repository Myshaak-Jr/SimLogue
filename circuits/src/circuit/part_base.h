#pragma once

#include <cstddef>
#include <functional>
#include <memory>

#include "./circuit_matrix.h"


class Node;
class Part;
class Ammeter;

struct Pin {
	size_t pin_id;
	Node* node;
	Part* owner;

	Pin(size_t pin_id, Node* node, Part* owner) : pin_id(pin_id), node(node), owner(owner) {}
};

struct ConstPin {
	size_t pin_id;
	const Node* node;
	const Part* owner;

	ConstPin(size_t pin_id, const Node* node, const Part* owner) : pin_id(pin_id), node(node), owner(owner) {}
};

class Part {
private:
	Ammeter* ammeter;

public:
	Part();

	virtual ~Part() = default;

	virtual constexpr std::size_t pin_count() const noexcept = 0;

	virtual void set_node(std::size_t i, Node* node) = 0;
	virtual Pin pin(std::size_t i) = 0;
	virtual ConstPin pin(std::size_t i) const = 0;

	virtual void stamp(CircuitMatrix& matrix) const = 0;

	virtual inline constexpr bool is_voltage_source() const { return false; }

	bool has_ammeter() const;
	Ammeter* get_ammeter();
	const Ammeter* get_ammeter() const;
	void set_ammeter(Ammeter* ammeter);
};
