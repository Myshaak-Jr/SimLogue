#pragma once

#include "./n_pin_part.h"


class Resistor : public NPinPart<2> {
private:
	double ohms;
	double conductance;

public:
	Resistor(double ohms);
	~Resistor() noexcept = default;

	void stamp(CircuitMatrix& matrix) const override;
};