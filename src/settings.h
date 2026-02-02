#pragma once

#include "circuit/scalar.h"

#include <filesystem>


namespace fs = std::filesystem;

struct Settings {
	scalar duration = 0.0;
	bool exit = false;
	int exit_code = 0;
	fs::path tables_path = fs::path("./tables/");
	scalar samplerate = 44100.0;
	fs::path circuit_path = fs::path("");
	bool export_tables = false;
	bool show_graphs = false;
};

Settings handle_args(int argc, char *argv[]);