#include <iostream>

#include "./circuit/circuit.h"
#include "./circuit/resistor.h"


int main() {
	Circuit circuit{};
	
	VoltageSource* ground = circuit.get_ground();
	VoltageSource* source = circuit.add_part<VoltageSource>(5.0f);

	Resistor* resistor = circuit.add_part<Resistor>(10.0f);

	circuit.connect(resistor->pin(0), ground->pin());

	/*resistor->connect(0, source->pin());
	resistor->connect(1, ground->pin());*/

	return 0;
}