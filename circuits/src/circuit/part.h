#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <string>

#include "circuit_matrix.h"
#include "node.h"
#include "pin.h"


struct StampParams {
	const ConstPin& ground;
	scalar timestep;
	scalar timestep_inv;
};


class Part {
public:
	Part() = default;
	virtual ~Part() = default;

	virtual size_t pin_count() const noexcept = 0;

	virtual void set_node(size_t pin_id, Node* node) = 0;
	virtual Pin pin(size_t pin_id) = 0;
	virtual ConstPin pin(size_t pin_id) const = 0;

	virtual void pre_stamp(CircuitMatrix& matrix, const StampParams& params) {}
	virtual void stamp(CircuitMatrix& matrix, const StampParams& params) const = 0;
	virtual void post_stamp(const CircuitMatrix& matrix, const StampParams& params) {};

	virtual const std::string& get_name() const = 0;
	virtual void set_name(const std::string& name) = 0;

	virtual scalar get_current_between(const ConstPin& a, const ConstPin& b) const = 0;
};
