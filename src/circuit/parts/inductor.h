#pragma once

#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include <string>


class Inductor : public NPinPart<2> {
private:
	scalar inductance;
	scalar last_i;
	size_t branch_id;

public:
	Inductor(const std::string &name, scalar inductance);
	~Inductor() noexcept = default;

	size_t num_needed_matrix_rows() const override { return 1; }
	void set_first_matrix_row_id(size_t row_id) override { branch_id = row_id; }
	size_t get_first_matrix_row_id() override { return branch_id; }

	std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(const StampParams &params) override;
	void stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	void update_value_from_result(size_t i, scalar value) override { last_i = value; }
};