#pragma once

#include "../lingebra.h"
#include "circuit_matrix.h"
#include "n_pin_part.h"
#include "node.h"
#include "part.h"
#include "parts/voltage_source.h"
#include "pin.h"
#include "scalar.h"
#include "scope.h"
#include <filesystem>
#include <memory>
#include <ranges>
#include <type_traits>
#include <vector>


class Circuit {
private:
	std::vector<std::unique_ptr<Node>> nodes;
	std::vector<std::unique_ptr<Part>> parts;

	VoltageSource *ground;

	std::vector<std::unique_ptr<VoltageScope>> voltage_scopes;
	std::vector<std::unique_ptr<CurrentScope>> current_scopes;

	lingebra::Permutation row_swap, col_swap;

	scalar timestep;
	fs::path scope_export_path;


	Node *create_new_node();
	void update();

	lingebra::MatrixCSC<scalar> build_matrix() const;

public:
	explicit Circuit(scalar timestep, const fs::path &scope_export_path = "./");
	~Circuit() noexcept = default;


	template <class TPart, class... TArgs>
		requires (std::is_base_of_v<Part, TPart>)
	TPart *add_part(TArgs&&... args) {
		auto part = std::make_unique<TPart>(std::forward<TArgs>(args)...);
		TPart *raw = part.get();
		parts.push_back(std::move(part));

		return raw;
	}

	VoltageSource *get_ground() const;

	void connect(const Pin &pin_a, const Pin &pin_b);

	inline void set_timestep(scalar dt) { timestep = dt; }
	inline scalar get_timestep() const { return timestep; }

	void scope_voltage(const ConstPin &a, const ConstPin &b);
	// Pin a and b must be of the same part or the single pin voltage source and ground pin
	void scope_current(const ConstPin &a, const ConstPin &b);
	inline void scope_current(const NPinPart<2> *part) {
		scope_current(part->pin(0), part->pin(1));
	}
	inline void scope_current(const VoltageSource *part) {
		scope_current(part->pin(0), get_ground()->pin(0));
	}

	void export_tables() const;

	void run_for_steps(size_t num_steps);
	void run_for_seconds(scalar secs);

	inline auto get_nodes() const {
		return nodes | std::views::transform([](const auto &x) -> const auto & { return *x; });
	}

	// return lazy iterator to nodes
	inline auto get_nodes() {
		return nodes | std::views::transform([](auto &x) -> auto & { return *x; });
	}

	// return lazy const iterator to parts
	inline auto get_parts() const {
		return parts | std::views::transform([](const auto &x) -> const auto & { return *x; });
	}

	// return lazy iterator to parts
	inline auto get_parts() {
		return parts | std::views::transform([](auto &x) -> auto & { return *x; });
	}
};
