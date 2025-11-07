#include "voltage_source.h"



VoltageSource::VoltageSource(double voltage) :
	voltage(voltage)
{}

VoltageSource::~VoltageSource() {}

double VoltageSource::get_voltage() const { return voltage; }

void VoltageSource::stamp(CircuitMatrix& matrix) const {
	matrix.stamp_voltage_source(get_ammeter()->get_row_id(), voltage);
}