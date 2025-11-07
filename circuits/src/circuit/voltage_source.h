#pragma once


#include "n_pin_part.h"


class VoltageSource : public NPinPart<1> {
private:
	double voltage;

public:
	explicit VoltageSource(double voltage);
	~VoltageSource() noexcept;

	double get_voltage() const;

	inline constexpr bool is_voltage_source() const override { return true; }

	void stamp(CircuitMatrix& matrix) const override;
};
