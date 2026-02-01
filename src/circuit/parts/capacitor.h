#pragma once

#include "circuit/n_pin_part.h"
#include "circuit/part.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <string>


class Capacitor : public NPinPart<2> {
private:
	scalar capacitance;
	scalar last_v;
	scalar last_i;
	scalar admittance;

public:
	Capacitor(const std::string &name, scalar capacitance);
	~Capacitor() noexcept = default;

	std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(const StampParams &params) override;
	void stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	void update(const StampParams &params) override;
};