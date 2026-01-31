#include "scope.h"

#include <format>
#include <fstream>

#include "part.h"
#include "pin.h"
#include "scalar.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>


Scope::Scope(const ConstPin &a, const ConstPin &b, const fs::path &export_path, const std::string &values_name) :
	a(a), b(b),
	export_path(export_path),
	values_name(values_name) {
	name = std::format("{}-between-{}-and-{}", values_name, a.name, b.name);
}

void Scope::export_table() const {
	fs::path filename = std::format("{}.csv", name);
	fs::path filepath = export_path / filename;
	std::ofstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open output file: " + filepath.string());
	}

	file << "time," << values_name << "\n";

	for (size_t i = 0; i < times.size(); ++i) {
		file << times[i] << "," << values[i] << "\n";
	}

	file.close();

	fs::remove(export_path.parent_path() / "latest" / filename);
	fs::copy_file(filepath, export_path.parent_path() / "latest" / filename);

	std::cout << "Exported " << values_name << " table " << filepath << "\n";
}

void Scope::plot(sciplot::Plot2D &p) const {
	using namespace sciplot;

	p.palette("paired");

	if constexpr (std::is_same_v<scalar, double>) {
		p.drawCurve(times, values);
	}
	else {
		std::vector<double> x(times.begin(), times.end());
		std::vector<double> y(values.begin(), values.end());

		p.drawCurve(x, y);
	}

	p.xlabel("time");
	p.ylabel(values_name);

	p.legend().hide();

	std::cout << "Plotted " << name << "\n";
}


VoltageScope::VoltageScope(const ConstPin &a, const ConstPin &b, const fs::path &export_path) :
	Scope(a, b, export_path, "voltage") {
}

void VoltageScope::record(scalar time) {
	scalar voltage = a.node->voltage - b.node->voltage;
	times.push_back(time);
	values.push_back(voltage);
}

CurrentScope::CurrentScope(const ConstPin &a, const ConstPin &b, const fs::path &export_path) :
	Scope(a, b, export_path, "current") {
	assert(a.owner == b.owner);
}

void CurrentScope::record(scalar time) {
	const Part *part = a.owner;
	scalar current = part->get_current_between(a, b);
	times.push_back(time);
	values.push_back(current);
}
