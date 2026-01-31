#pragma once

#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include <string>


class Resistor : public NPinPart<2> {
private:
	scalar ohms;
	scalar conductance;

public:
	Resistor(const std::string &name, scalar ohms);
	~Resistor() noexcept = default;

	std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(const StampParams &params) override;
	void stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) override {}

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;
};