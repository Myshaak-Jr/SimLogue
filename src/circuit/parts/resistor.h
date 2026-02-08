#pragma once

#include "circuit/n_pin_part.h"
#include "circuit/part.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <string>


class Resistor : public NPinPart<2> {
private:
	scalar ohms;
	scalar conductance;

public:
	Resistor(const std::string &name, scalar ohms);
	~Resistor() noexcept = default;

	std::vector<MatrixEntry> gen_matrix_entries(const StampParams &params) override;
	void stamp_rhs_entries([[maybe_unused]] std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) override {}

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;
};