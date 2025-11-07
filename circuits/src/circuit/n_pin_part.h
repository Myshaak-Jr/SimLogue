#pragma once

#include <array>
#include <stdexcept>

#include "node.h"


template <std::size_t N>
class NPinPart : public Part {
private:
	std::array<Node*, N> nodes{};

public:
	NPinPart() {
		nodes.fill(nullptr);
	};
	~NPinPart() noexcept = default;

	constexpr std::size_t pin_count() const noexcept override { return N; }

	void set_node(std::size_t i, Node* node) override {
		nodes[i] = node;
	}

	Pin pin(std::size_t i) override {
		return Pin{i, nodes[i], this};
	}

	ConstPin pin(std::size_t i) const override {
		return ConstPin{i, nodes[i], this};
	}
};


template <>
class NPinPart<1> : public Part {
private:
	Node* node;

public:
	NPinPart() : node(nullptr) {}
	~NPinPart() noexcept = default;

	constexpr std::size_t pin_count() const noexcept override { return 1; }

	void set_node(std::size_t i, Node* new_node) override {
		if (i != 0)
			throw std::out_of_range("NPinPart<1> has just one pin");

		node = new_node;
	}
		
	Pin pin(std::size_t i) override {
		if (i != 0)
			throw std::out_of_range("NPinPart<1> has just one pin");

		return Pin{i, node, this};
	}

	ConstPin pin(std::size_t i) const override {
		return ConstPin{i, node, this};
	}

	Pin pin() { return pin(0); }

	ConstPin pin() const { return pin(0); }
};