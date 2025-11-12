#include <iostream>

#include "./circuit/circuit.h"
#include "./circuit/resistor.h"


int main() {
	Circuit circuit{};

	VoltageSource* ground = circuit.get_ground();
	VoltageSource* source = circuit.add_part<VoltageSource>(5.0f);

	Resistor* r1 = circuit.add_part<Resistor>(10.0f);
	Resistor* r2 = circuit.add_part<Resistor>(10.0f);

	circuit.connect(r1->pin(0), source->pin());
	circuit.connect(r1->pin(1), r2->pin(0));
	circuit.connect(r2->pin(1), ground->pin());
	
	circuit.run_for(1);
	
	circuit.voltage_on_pin(r1->pin(1));

	return 0;
}