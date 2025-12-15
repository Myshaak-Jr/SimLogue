#pragma once

#include "../n_pin_part.h"


class CurrentSource : public NPinPart<2> {
private:
	scalar current;

public:
	CurrentSource(const std::string& name, scalar current);
	~CurrentSource() noexcept = default;

	void stamp(CircuitMatrix& matrix, const StampParams& params) const override;

	scalar get_current_between(const ConstPin& a, const ConstPin& b) const override;
};