#pragma once

#include <memory>
#include <vector>

#include "./part_base.h"
#include "./node.h"
#include "./voltage_source.h"
#include "./circuit_matrix.h"


class Circuit {
private:
	std::vector<std::unique_ptr<Node>> nodes;
	std::vector<std::unique_ptr<Part>> parts;

	std::vector<VoltageSource*> sources;

	VoltageSource* ground;

	Node* create_new_node();

	CircuitMatrix matrix;

	double timestep;

	void update();

public:
	explicit Circuit(double timestep);
	~Circuit() noexcept = default;


	template <class TPart, class... TArgs>
	TPart* add_part(TArgs&&... args) {
		static_assert(std::is_base_of_v<Part, TPart>, "TPart must derive from Part.");
		
		auto part = std::make_unique<TPart>(std::forward<TArgs>(args)...);
		TPart* raw = part.get();
		parts.push_back(std::move(part));

		if constexpr (std::is_same_v<TPart, VoltageSource>) {
			sources.push_back(raw);
		}

		return raw;
	}

	VoltageSource* get_ground() const;

	void connect(Pin pin_a, Pin pin_b);

	inline void set_timestep(double dt) { timestep = dt; }
	inline double get_timestep() const { return timestep; }


	double get_voltage_on_pin(Pin pin) const;

	void run_for(size_t num_steps);
};