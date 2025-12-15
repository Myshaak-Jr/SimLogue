#pragma once

#include <array>
#include <stdexcept>
#include <format>

#include "node.h"
#include "part.h"


template <size_t N>
class NPinPart : public Part {
private:
	std::array<Node*, N> nodes;

	std::string name;

protected:
	std::array<std::string, N> pin_names;

	virtual void update_pin_names() {
		for (size_t i = 0; i < N; ++i) {
			pin_names[i] = std::format("{}-pin{}", name, i);
		}
	}

	void assert_pin_id(size_t pin_id) const {
		if (pin_id >= N) {
			throw std::out_of_range(std::format("NPinPart<{}> does not have pin {}", N, pin_id));
		}
	}

public:
	NPinPart(const std::string& name) : name(name) {
		nodes.fill(nullptr);
		update_pin_names();
	};
	~NPinPart() noexcept = default;

	constexpr size_t pin_count() const noexcept override { return N; }

	void set_node(size_t pin_id, Node* node) override {
		assert_pin_id(pin_id);
		nodes[pin_id] = node;
	}

	Pin pin(size_t pin_id) override {
		assert_pin_id(pin_id);
		return Pin{pin_id, nodes[pin_id], this, pin_names[pin_id]};
	}

	ConstPin pin(size_t pin_id) const override {
		assert_pin_id(pin_id);
		return ConstPin{pin_id, nodes[pin_id], this, pin_names[pin_id]};
	}

	const std::string& get_name() const override { return name; }

	void set_name(const std::string& name) override {
		this->name = name;
		update_pin_names();
	}

	Pin pin() requires(N == 1) { return pin(0); }
	ConstPin pin() const requires(N == 1) { return pin(0); }

	scalar get_current() const requires(N == 2) { return get_current_between(pin(0), pin(1)); }
};
