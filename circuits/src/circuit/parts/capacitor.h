#pragma once

#include "../n_pin_part.h"


class Capacitor : public NPinPart<2> {
private:
	scalar capacitance;
	scalar last_v;
	scalar admittance;

public:
	Capacitor(const std::string& name, scalar capacitance);
	~Capacitor() noexcept = default;

	void pre_stamp(CircuitMatrix& matrix, const StampParams& params) override;
	void stamp(CircuitMatrix& matrix, const StampParams& params) const override;
	void post_stamp(const CircuitMatrix& matrix, const StampParams& params) override;

	scalar get_current_between(const ConstPin& a, const ConstPin& b) const override;
};