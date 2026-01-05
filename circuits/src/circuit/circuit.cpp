#include "../lingebra.h"
#include "circuit.h"
#include "node.h"
#include "part.h"
#include "parts/voltage_source.h"
#include "pin.h"
#include "scalar.h"
#include "scope.h"
#include "util.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <tuple>
#include <utility>


Circuit::Circuit(scalar timestep, const fs::path &scope_export_path) :
	timestep(timestep),
	matrix(),
	scope_export_path(scope_export_path / make_timestamp()) {
	fs::create_directories(this->scope_export_path);
	fs::create_directories(scope_export_path / "latest");
	ground = add_part<VoltageSource>("GND", 0.0f);
	Node *ground_node = create_new_node();
	ground_node->is_ground = true;
	ground->set_node(0, ground_node);
}


VoltageSource *Circuit::get_ground() const {
	return ground;
}

Node *Circuit::create_new_node() {
	auto node = std::make_unique<Node>();
	Node *raw = node.get();
	nodes.push_back(std::move(node));
	return raw;
}

void Circuit::connect(const Pin &pin_a, const Pin &pin_b) {
	if (pin_a.node == pin_b.node) {
		if (pin_a.node == nullptr) {
			Node *node = create_new_node();
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
	for (auto &node : nodes) {
		if (node->is_ground) continue;
		node->node_id = matrix.reserve_row();
	}
	for (auto &part : parts) {
		part->pre_stamp(matrix, params);
	}

	matrix.init();

	// fill the matrix
	for (const auto &part : parts) {
		part->stamp(matrix, params);
	}

	matrix.solve();

	for (auto &node : nodes) {
		if (node->is_ground) continue;
		node->voltage = matrix.get_solution_value(node->node_id);
	}

	for (auto &part : parts) {
		part->post_stamp(matrix, params);
	}
}

lingebra::MatrixCSC<scalar> Circuit::build_matrix() const {
	// reserve rows
	size_t num_rows = 0;

	for (auto &node : nodes) {
		node->node_id = num_rows++;
	}

	for (auto &part : parts) {
		part->set_first_matrix_row_id(num_rows);
		num_rows += part->num_needed_matrix_rows();
	}

	// [(row, column, data), ...]
	std::vector<std::tuple<size_t, size_t, scalar>> matrix_entries;

	for (const auto &part : parts) {
		matrix_entries.append_range(part->gen_matrix_entries());
	}

	matrix_entries = std::move(counting_sort(matrix_entries, num_rows, [](const auto &a, const auto &b) {return std::get<0>(a) < std::get<0>(b); }));
	matrix_entries = std::move(counting_sort(matrix_entries, num_rows, [](const auto &a, const auto &b) {return std::get<1>(a) < std::get<1>(b); }));

	std::vector<scalar> data;
	std::vector<size_t> rows, ptrs;

	size_t last_col = 0, last_row = 0;
	ptrs.push_back(0);

	for (const auto &[row, col, datum] : matrix_entries) {
		if (col == last_col && row == last_row) {
			data[data.size() - 1] += datum;
		}
		else {
			data.push_back(datum);
			rows.push_back(row);
		}

		if (col != last_col) {
			ptrs.push_back(data.size());
		}

		last_row = row;
		last_col = col;
	}

	return lingebra::MatrixCSC<scalar>(num_rows, num_rows, std::move(data), std::move(rows), std::move(ptrs));
}

void Circuit::update() {

}

void Circuit::run_for_steps(size_t num_steps) {
	auto matrix = build_matrix();

	// pivot
	std::tie(row_swap, col_swap) = lingebra::generate_pivoting(matrix);
	lingebra::reorder_rows(matrix, row_swap);
	lingebra::reorder_cols(matrix, col_swap);


	std::cout << "Running for " << num_steps << " steps\n";

	size_t step = 0;
	scalar t = 0;

	try {
		for (; step < num_steps; ++step) {
			update();
		}
	}
	catch (const lingebra::singular_matrix_exception &) {
		std::cout << "Singular matrix encountered at time=" << t << "(step=" << step << ")\n";
	}

	//try {
	//	for (; step < num_steps; ++step) {
	//		update_parts();

	//		for (const auto &scope : voltage_scopes) {
	//			scope->record_voltage(t);
	//		}
	//		for (const auto &scope : current_scopes) {
	//			scope->record_current(t);
	//		}

	//		t += timestep;
	//	}
	//}
	//catch (const lingebra::singular_matrix_exception &) {
	//	std::cout << "Singular matrix encountered at time=" << t << "(step=" << step << ")\n";
	//}
}

void Circuit::run_for_seconds(scalar secs) {
	run_for_steps(static_cast<size_t>(secs / timestep));
}

// scopes
void Circuit::scope_voltage(const ConstPin &a, const ConstPin &b) {
	voltage_scopes.push_back(std::make_unique<VoltageScope>(a, b, scope_export_path));
}

void Circuit::scope_current(const ConstPin &a, const ConstPin &b) {
	current_scopes.push_back(std::make_unique<CurrentScope>(a, b, scope_export_path));
}

void Circuit::export_tables() const {
	std::cout << "Exporting tables...\n";

	for (const auto &scope : voltage_scopes) {
		scope->export_data();
	}
	for (const auto &scope : current_scopes) {
		scope->export_data();
	}
}
