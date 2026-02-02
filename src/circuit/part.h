#pragma once

#include "circuit/node.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <string>
#include <tuple>
#include <vector>


struct StampParams {
	const ConstPin &ground;
	scalar timestep;
	scalar timestep_inv;
	size_t step;
	scalar time;
};


class Part {
public:
	Part() = default;
	virtual ~Part() = default;

	virtual size_t pin_count() const noexcept = 0;

	virtual void set_node(size_t pin_id, Node *node) = 0;
	virtual Pin pin(size_t pin_id) = 0;
	virtual ConstPin pin(size_t pin_id) const = 0;

	virtual Pin pin(const std::string &pinname) = 0;
	virtual ConstPin pin(const std::string &pinname) const = 0;

	virtual size_t num_needed_matrix_rows() const { return 0; };
	virtual void set_first_matrix_row_id([[maybe_unused]] size_t first_row_id) {}
	virtual size_t get_first_matrix_row_id() { return 0; };

	virtual std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(const StampParams &params) = 0;
	virtual void stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) = 0;

	virtual const std::string &get_name() const = 0;
	virtual void set_name(const std::string &name) = 0;

	virtual scalar get_current_between(const ConstPin &a, const ConstPin &b) const = 0;

	// i in range 0 .. num_needed_matrix_rows() - 1
	virtual void update_value_from_result([[maybe_unused]] size_t i, [[maybe_unused]] scalar value) {}

	virtual void update([[maybe_unused]] const StampParams &params) {};
};