#include <iostream>

#include "circuit/circuit.h"
#include "circuit/parts/resistor.h"


int main() {
	Circuit circuit{1e-3, "D:/tables"};

	VoltageSource* ground = circuit.get_ground();
	VoltageSource* source = circuit.add_part<VoltageSource>("V1", 5.0f);

	Resistor* r1 = circuit.add_part<Resistor>("R1", 10.0f);
	Resistor* r2 = circuit.add_part<Resistor>("R2", 10.0f);

	circuit.connect(r1->pin(0), source->pin());
	circuit.connect(r1->pin(1), r2->pin(0));
	circuit.connect(r2->pin(1), ground->pin());
	
	circuit.scope_voltage(r1->pin(1), ground->pin());
	circuit.scope_current(r1);

	circuit.run_for_steps(100);

	circuit.export_tables();
	
	return 0;
}