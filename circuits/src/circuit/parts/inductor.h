#pragma once

#include "../n_pin_part.h"


class Inductor : public NPinPart<2> {
private:
	scalar inductance;
	scalar last_i;
	size_t branch_id;

public:
	Inductor(const std::string& name, scalar inductance);
	~Inductor() noexcept = default;

	void pre_stamp(CircuitMatrix& matrix, const StampParams& params) override;
	void stamp(CircuitMatrix& matrix, const StampParams& params) const override;
	void post_stamp(const CircuitMatrix& matrix, const StampParams& params) override;

	scalar get_current_between(const ConstPin& a, const ConstPin& b) const override;
};