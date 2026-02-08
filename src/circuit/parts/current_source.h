#pragma once

#include "circuit/n_pin_part.h"
#include "circuit/part.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <string>


class CurrentSource : public NPinPart<2> {
private:
	scalar current;

public:
	CurrentSource(const std::string &name, scalar current);
	~CurrentSource() noexcept = default;

	std::vector<MatrixEntry> gen_matrix_entries([[maybe_unused]] const StampParams &params) override { return {}; }
	void stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;
};