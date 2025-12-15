#include "current_source.h"


CurrentSource::CurrentSource(const std::string& name, scalar current) :
	NPinPart<2>(name),
	current(current)
{}

void CurrentSource::stamp(CircuitMatrix& matrix, const StampParams& params) const {
	matrix.stamp_template_RHS(pin(0), pin(1), current);
}

scalar CurrentSource::get_current_between(const ConstPin& a, const ConstPin& b) const {
	return current;
}

