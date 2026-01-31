#pragma once

#include "node.h"
#include "part.h"
#include <array>
#include <format>
#include <ranges>
#include <stdexcept>


template <size_t N>
class NPinPart : public Part {
private:
	std::array<Node *, N> nodes;

	std::string name;

protected:
	std::array<std::string, N> pin_names;

	void assert_pin_id(size_t pin_id) const {
		if (pin_id >= N) {
			throw std::out_of_range(std::format("NPinPart<{}> does not have pin {}", N, pin_id));
		}
	}

	virtual std::string get_pin_name(size_t pin_id) const noexcept {
		return pin_names[pin_id];
	}

public:
	NPinPart(const std::string &name) : name(name) {
		nodes.fill(nullptr);

		for (size_t i = 0; i < N; ++i) {
			pin_names[i] = std::format("{}", static_cast<char>('a' + i));
		}
	};
	~NPinPart() noexcept = default;

	constexpr size_t pin_count() const noexcept override { return N; }

	void set_node(size_t pin_id, Node *node) override {
		assert_pin_id(pin_id);
		nodes[pin_id] = node;
	}

	Pin pin(size_t pin_id) override {
		assert_pin_id(pin_id);
		return Pin(pin_id, nodes[pin_id], this, std::format("{}.{}", name, get_pin_name(pin_id)));
	}

	ConstPin pin(size_t pin_id) const override {
		assert_pin_id(pin_id);
		return ConstPin(pin_id, nodes[pin_id], this, std::format("{}.{}", name, get_pin_name(pin_id)));
	}

	Pin pin(const std::string &pinname) override {
		for (size_t i = 0; i < pin_names.size(); ++i) {
			if (pin_names[i] == pinname) return pin(i);
		}

		throw std::out_of_range(std::format("NPinPart<{}> does not have pin {}", N, pinname));
	}

	ConstPin pin(const std::string &pinname) const override {
		for (size_t i = 0; i < pin_names.size(); ++i) {
			if (pin_names[i] == pinname) return pin(i);
		}

		throw std::out_of_range(std::format("NPinPart<{}> does not have pin {}", N, pinname));
	}

	const std::string &get_name() const override { return name; }

	void set_name(const std::string &name) override {
		this->name = name;
	}

	Pin pin() requires(N == 1) { return pin(0); }
	ConstPin pin() const requires(N == 1) { return pin(0); }

	scalar get_current() const requires(N == 2) { return get_current_between(pin(0), pin(1)); }
};
