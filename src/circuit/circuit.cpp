#include "circuit/circuit.h"

#include "circuit/interpreter/interpreter.h"
#include "circuit/node.h"
#include "circuit/part.h"
#include "circuit/parts/voltage_source.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"
#include "circuit/scope.h"
#include "circuit/util.h"
#include "lingebra/lingebra.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>



Circuit::Circuit(scalar timestep, const fs::path &scope_export_path) :
	timestep(timestep),
	scope_export_path(scope_export_path / make_timestamp()),
	interpreter(nullptr) {
	fs::create_directories(this->scope_export_path);
	fs::create_directories(scope_export_path / "latest");
	ground = add_part<VoltageSource>("GND", 0.0f);
	Node *ground_node = create_new_node();
	ground_node->is_ground = true;
	ground->set_node(0, ground_node);
	interpreter = std::unique_ptr<Interpreter>(new Interpreter(*this));
}

Circuit::~Circuit() noexcept {}


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

lingebra::Matrix<scalar> Circuit::build_matrix(const StampParams &params) const {
	// reserve rows
	size_t num_rows = 0;

	for (auto &node : nodes) {
		if (node->is_ground) continue;
		node->node_id = num_rows++;
	}

	for (auto &part : parts) {
		part->set_first_matrix_row_id(num_rows);
		num_rows += part->num_needed_matrix_rows();
	}

	// [(row, column, data), ...]
	std::vector<std::tuple<size_t, size_t, scalar>> matrix_entries;

	for (const auto &part : parts) {
		auto entries = part->gen_matrix_entries(params);
		matrix_entries.insert(matrix_entries.end(), entries.begin(), entries.end());
	}

	lingebra::Matrix<scalar> matrix(num_rows, num_rows);

	for (const auto &[row, col, value] : matrix_entries) {
		matrix(row, col) += value;
	}

	return matrix;
}

void Circuit::update(size_t step, scalar t) {
	StampParams params{
		.ground = ground->pin(),
		.timestep = timestep,
		.timestep_inv = 1.0 / timestep,
		.step = step,
		.time = t,
	};

	// TODO: update the matrix instead of building it anew
	auto matrix = build_matrix(params);

	std::vector<scalar> rhs(matrix.m(), 0.0);

	for (auto &part : parts) {
		part->stamp_rhs_entries(rhs, params);
	}

	lingebra::Vector<scalar> rhs_vec(rhs); // TODO: do not copy the vector, just move it insted

	//std::cout << matrix.repr() << "\n" << rhs_vec.repr() << "\n";

	lingebra::solve_gaussian_elimination(matrix, rhs_vec);

	for (auto &part : parts) {
		for (size_t i = 0; i < part->num_needed_matrix_rows(); ++i) {
			part->update_value_from_result(i, rhs_vec[part->get_first_matrix_row_id() + i]);
		}
	}
	for (auto &node : nodes) {
		if (node->is_ground) continue;
		node->voltage = rhs_vec[node->node_id];
	}

	for (auto &part : parts) {
		part->update(params);
	}
}

void Circuit::run_for_steps(size_t num_steps) {
	// TODO: do LU decomposition

	std::cout << "Running for " << num_steps << " steps" << std::endl;

	size_t step = 0;
	scalar t = 0;

	try {
		for (; step < num_steps; ++step) {
			update(step, t);

			for (const auto &scope : scopes) {
				scope->record(t);
			}

			t += timestep;
		}
	}
	catch (const lingebra::singular_matrix_exception &) {
		std::cout << "Singular matrix encountered at time=" << t << "(step=" << step << ")\n";
	}
}

void Circuit::run_for_seconds(scalar secs) {
	run_for_steps(static_cast<size_t>(secs / timestep));
}

// scopes
void Circuit::scope_voltage(const ConstPin &a, const ConstPin &b) {
	scopes.push_back(std::make_unique<VoltageScope>(a, b, scope_export_path));
}

void Circuit::scope_current(const ConstPin &a, const ConstPin &b) {
	scopes.push_back(std::make_unique<CurrentScope>(a, b, scope_export_path));
}

void Circuit::export_tables() const {
	std::cout << "Exporting tables..." << std::endl;

	for (const auto &scope : scopes) {
		scope->export_table();
	}
}

void Circuit::show_graphs() const {
	using sciplot::PlotVariant;
	using sciplot::Plot2D;
	using sciplot::Figure;
	using sciplot::Canvas;

	// calculate the plot grid dimensions to be 16:9
	size_t n = scopes.size();

	size_t w = ceil_sqrt(n) * 4 / 3;
	size_t h = (n + w - 1) / w;

	std::vector<std::vector<PlotVariant>> plot_grid(h, std::vector<PlotVariant>(w));

	for (size_t i = 0; i < n; ++i) {
		scopes[i]->plot(std::get<Plot2D>(plot_grid[i / w][i % w]));
	}

	Figure figure(plot_grid);
	Canvas canvas{ {figure} };
	canvas.size(1920 * 3 / 5, 1080 * 3 / 5);

	canvas.defaultPalette("set1");

	canvas.show();
}

void Circuit::load_circuit(const fs::path &script) {
	std::cout << "Loading circuit " << script << std::endl;

	std::ifstream f(script);

	if (!f) {
		throw std::runtime_error("Cannot open script file: " + script.string());
	}

	interpreter->execute(f);
}