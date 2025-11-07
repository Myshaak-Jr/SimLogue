#include "./resistor.h"


Resistor::Resistor(double ohms) : ohms(ohms) {
	conductance = 1.0f / ohms;
}

void Resistor::stamp(CircuitMatrix& matrix) const {
	matrix.stamp_conductance(pin(0).node->get_row_id(), pin(1).node->get_row_id(), conductance);
}