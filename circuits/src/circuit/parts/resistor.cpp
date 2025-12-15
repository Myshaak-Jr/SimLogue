#include "./resistor.h"


Resistor::Resistor(const std::string& name, scalar ohms) : NPinPart<2>(name), ohms(ohms) {
	conductance = 1.0f / ohms;
}

void Resistor::stamp(CircuitMatrix& matrix, const StampParams& params) const {
	matrix.stamp_template_LHS(pin(0), pin(1), conductance);
}

scalar Resistor::get_current_between(const ConstPin& a, const ConstPin& b) const {
	if (a.owner != this || b.owner != this) {
		throw std::runtime_error("Pins a and b must belong to this part.");
	}
	return conductance * (a.node->voltage - b.node->voltage);
}
