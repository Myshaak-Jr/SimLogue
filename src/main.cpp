#include <iostream>

#include "settings.h"

#include "circuit/circuit.h"
#include "circuit/interpreter/interpreter.h"
#include "circuit/parts/capacitor.h"
#include "circuit/parts/resistor.h"
#include "circuit/parts/switch.h"
#include "circuit/parts/voltage_source.h"
#include "circuit/scalar.h"

#ifdef _WIN32
#include <Windows.h>
#endif


// TODO: some mutlishow plt error idk idc, it works


int main(int argc, char *argv[]) {
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif

	// handle arguments
	Settings settings = handle_args(argc, argv);
	if (settings.exit) return settings.exit_code;


	Circuit circuit(1.0_s / settings.samplerate, settings.tables_path);

	try {
		circuit.load_circuit(settings.circuit_path);
		circuit.run_for_seconds(settings.duration);

		if (settings.export_tables) circuit.export_tables();
		if (settings.show_graphs) circuit.show_graphs();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}