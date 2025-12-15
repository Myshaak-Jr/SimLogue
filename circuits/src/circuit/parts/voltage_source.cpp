#include "voltage_source.h"



VoltageSource::VoltageSource(const std::string& name, scalar voltage) : NPinPart<1>(name), voltage(voltage), branch_id(0), current(0) {}

VoltageSource::~VoltageSource() {}

void VoltageSource::stamp(CircuitMatrix& matrix, const StampParams& params) const {
	matrix.stamp_template_I_out_LHS(pin(), params.ground, branch_id, 1.0, 0.0);
	matrix.stamp_template_RHS(branch_id, voltage);
}

void VoltageSource::pre_stamp(CircuitMatrix& matrix, const StampParams& params) {
	branch_id = matrix.reserve_row();
}

void VoltageSource::post_stamp(const CircuitMatrix& matrix, const StampParams& params) {
	current = matrix.get_solution_value(branch_id);
}

scalar VoltageSource::get_current_between(const ConstPin& a, const ConstPin& b) const {
	if (a.owner != this) {
		throw std::runtime_error("Pin a must belong to this part.");
	}
	else if (!b.node->is_ground) {
		throw std::runtime_error("Pin b must be a ground pin.");
	}

	return current;
}


VoltageSource2Pin::VoltageSource2Pin(const std::string& name, scalar voltage) : NPinPart<2>(name), voltage(voltage), branch_id(0), current(0) {}

VoltageSource2Pin::~VoltageSource2Pin() {}

void VoltageSource2Pin::stamp(CircuitMatrix& matrix, const StampParams& params) const {
	matrix.stamp_template_I_out_LHS(pin(0), pin(1), branch_id, 1.0, 0.0);
	matrix.stamp_template_RHS(branch_id, voltage);
}

void VoltageSource2Pin::pre_stamp(CircuitMatrix& matrix, const StampParams& params) {
	branch_id = matrix.reserve_row();
}

void VoltageSource2Pin::post_stamp(const CircuitMatrix& matrix, const StampParams& params) {
	current = matrix.get_solution_value(branch_id);
}

scalar VoltageSource2Pin::get_current_between(const ConstPin& a, const ConstPin& b) const {
	if (a.owner != this || b.owner != this) {
		throw std::runtime_error("Pins a and b must belong to this part.");
	}
	return current;
}