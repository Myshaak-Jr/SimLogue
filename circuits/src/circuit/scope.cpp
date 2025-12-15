#include "scope.h"

#include <fstream>
#include <format>

#include "part.h"


VoltageScope::VoltageScope(const ConstPin& a, const ConstPin& b, const fs::path& export_path) : a(a), b(b), export_path(export_path) {}

void VoltageScope::record_voltage(scalar time) {
	scalar voltage = a.node->voltage - b.node->voltage;
	data.push_back(DataEntry(time, voltage));
}

void VoltageScope::export_data() const {
	fs::path filepath = export_path / std::format("voltage-between-{}-and-{}.csv", a.name, b.name);
	std::ofstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open output file: " + filepath.string());
	}

	file << "Time, Voltage\n";

	for (const DataEntry& entry : data) {
		file << entry.time << ", " << entry.voltage << "\n";
	}
}


CurrentScope::CurrentScope(const ConstPin& a, const ConstPin& b, const fs::path& export_path) : a(a), b(b), export_path(export_path) {}

void CurrentScope::record_current(scalar time) {
	const Part* part = a.owner;

	scalar current = part->get_current_between(a, b);
	data.push_back(DataEntry(time, current));
}

void CurrentScope::export_data() const {
	fs::path filepath = export_path / std::format("current-between-{}-and-{}.csv", a.name, b.name);
	std::ofstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open output file: " + filepath.string());
	}

	file << "Time, Current\n";

	for (const DataEntry& entry : data) {
		file << entry.time << ", " << entry.current << "\n";
	}

	file.close();
}
