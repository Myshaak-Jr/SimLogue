#include "settings.h"

#include "circuit/interpreter/interpreter.h"
#include "version.h"

#include <filesystem>
#include <iostream>
#include <string>


static void print_help() {
	std::cout
		<< "SimLogue: An Analogue Circiut Simulator\n"
		<< "Version: " << simlogue::version::major << "."
		<< simlogue::version::minor << "."
		<< simlogue::version::patch << "\n\n"

		<< "Usage:\n"
		<< "  simlogue [options] circuit_file duration\n\n"

		<< "  circuit_file       .simlog file to load the circuit from\n"
		<< "  duration           Time value (see readme) specifying the run time\n\n"

		<< "Options:\n"
		<< "  -t, --tables     <path>   Path to generated CSV tables\n"
		<< "                            (default: ./tables/)\n"
		<< "  -v, --version             Show version information\n"
		<< "  -h, --help			    Show this help message\n"
		<< "  -r, --samplerate <freq>   Sets the samplerate in Hz\n"
		<< "                            (default: 44100)\n"
		<< "  -e, --export-tables       Exports the scope tables\n"
		<< "  -g, --show-graphs         Displays the scope graphs after run\n\n"
		;
}

static void print_version() {
	std::cout << "Version: " << simlogue::version::major << "." << simlogue::version::minor << "." << simlogue::version::patch << "\n";
}

Settings handle_args(int argc, char *argv[]) {
	Settings settings;

	bool do_print_help = false;
	bool do_print_version = false;

	bool accept_options = true;

	bool read_duration = false;

	size_t pos_idx = 0;

	for (int i = 1; i < argc; ++i) {
		std::string option(argv[i]);

		if (accept_options && (option == "-h" || option == "--help")) {
			do_print_help = true;
		}
		else if (accept_options && (option == "-v" || option == "--version")) {
			do_print_version = true;
		}
		else if (accept_options && (option == "-e" || option == "--export-tables")) {
			settings.export_tables = true;
		}
		else if (accept_options && (option == "-g" || option == "--show_graphs")) {
			settings.show_graphs = true;
		}
		else if (accept_options && (option == "-t" || option == "--tables")) {
			if (++i >= argc) {
				std::cout << "Option " << option << " requires <path> argument.\nSee help:\n\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
			std::string argument = argv[i];
			settings.tables_path = fs::path(argument);
		}
		else if (accept_options && (option == "-r" || option == "--samplerate")) {
			if (++i >= argc) {
				std::cout << "Option " << option << " requires <freq> argument.\nSee help:\n\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
			try {
				settings.samplerate = std::stof(argv[i]);
			}
			catch (const std::exception &) {
				std::cout << "Argument <freq> must be a floating point number in valid range.\nSee help:\n\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
			if (settings.samplerate <= 0.0) {
				std::cout << "Argument <freq> must be positive.\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
		}
		else {
			if (pos_idx == 0) {
				settings.circuit_path = option;
			}
			else if (pos_idx == 1) {
				try {
					auto [quantity, duration] = Interpreter::parse_value(argv[i], "in param duration");

					if (quantity != Quantity::Time) {
						throw ParseError(std::format("Value error in param duration: Duration has to be a time value, got value of type '{}'.", quantity_to_string(quantity)));
					}

					settings.duration = duration;
				}
				catch (const std::exception &e) {
					std::cout << e.what() << "\nSee help: \n\n";
					print_help();
					return Settings{ .exit = true, .exit_code = 2 };
				}
				if (settings.duration <= 0.0) { // for now the parse_value doesn't even support negatives, but just in case :D
					std::cout << "Argument duration must be positive.\n";
					print_help();
					return Settings{ .exit = true, .exit_code = 2 };
				}
				read_duration = true;
			}
			else {
				std::cout << "SimLogue accepts just 2 positional argument.\nSee help:\n\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}

			++pos_idx;
		}
	}

	if (do_print_help) {
		print_help();
		return Settings{ .exit = true, .exit_code = 0 };
	}
	if (do_print_version) {
		print_version();
		return Settings{ .exit = true, .exit_code = 0 };
	}

	if (settings.circuit_path == "") {
		std::cout << "SimLogue requires the circuit file path.\nSee help:\n\n";
		print_help();
		return Settings{ .exit = true, .exit_code = 2 };
	}
	if (!read_duration) {
		std::cout << "SimLogue requires the duration.\nSee help:\n\n";
		print_help();
		return Settings{ .exit = true, .exit_code = 2 };
	}

	return settings;
}