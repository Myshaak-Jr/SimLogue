#include <iostream>

#include "settings.h"

#include "circuit/circuit.h"
#include "circuit/interpreter/interpreter.h"
#include "circuit/parts/capacitor.h"
#include "circuit/parts/resistor.h"
#include "circuit/parts/switch.h"
#include "circuit/parts/voltage_source.h"
#include "circuit/scalar.h"


int main(int argc, char *argv[]) {
	// handle arguments
	Settings settings = handle_args(argc, argv);
	if (settings.exit) return settings.exit_code;


	Circuit circuit(1e-5, settings.tables_path);

	try {
		circuit.load_circuit(settings.circuit_path);
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
		return 1;
	}

	circuit.run_for_seconds(settings.duration);

	if (settings.export_tables) circuit.export_tables();
	if (settings.show_graphs) circuit.show_graphs();

	return 0;

	// Main code
	//Circuit circuit(1e-5, settings.tables_path);

	//VoltageSource *ground = circuit.get_ground();
	//VoltageSource *source = circuit.add_part<VoltageSource>("V1", 5);

	//Resistor *R1 = circuit.add_part<Resistor>("R1", 1_k);
	//Capacitor *C1 = circuit.add_part<Capacitor>("C1", 5_u);
	//Switch *S1 = circuit.add_part<Switch>("S1");

	//circuit.connect(source->pin(), S1->pin(0));
	//circuit.connect(R1->pin(0), S1->pin(1));
	//circuit.connect(R1->pin(1), C1->pin(0));
	//circuit.connect(C1->pin(1), ground->pin());

	//circuit.scope_voltage(C1->pin(0), C1->pin(1));
	//circuit.scope_current(C1);

	//S1->schedule_on(100_m / circuit.get_timestep());

	//std::cout << "Node Count: " << circuit.get_nodes().size() << "\n";
	//std::cout << "Part Count: " << circuit.get_parts().size() << "\n";

	//circuit.run_for_seconds(500_m);

	//circuit.export_tables();
	//circuit.show_graphs();

	return 0;
}