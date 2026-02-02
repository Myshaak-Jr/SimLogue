#pragma once

#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <filesystem>
#include <memory>
#include <sciplot/sciplot.hpp>
#include <vector>


namespace fs = std::filesystem;


class Scope {
private:
	fs::path export_path;

protected:
	std::vector<scalar> times;
	std::vector<scalar> values;

	ConstPin a;
	ConstPin b;

	std::string values_name;
	std::string name;

public:
	Scope(const ConstPin &a, const ConstPin &b, const fs::path &export_path, const std::string &values_name);
	virtual ~Scope() = default;

	// TODO: add reserve method to reserve memory before the run

	virtual void record(scalar time) = 0;

	void export_table() const;
	void plot(sciplot::Plot2D &p) const;
};


class VoltageScope : public Scope {
public:
	VoltageScope(const ConstPin &a, const ConstPin &b, const fs::path &export_path);

	void record(scalar time) override;
};

class CurrentScope : public Scope {
public:
	CurrentScope(const ConstPin &a, const ConstPin &b, const fs::path &export_path);

	void record(scalar time) override;
};