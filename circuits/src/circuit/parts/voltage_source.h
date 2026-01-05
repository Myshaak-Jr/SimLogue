#pragma once


#include "../circuit_matrix.h"
#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include <string>


class VoltageSource : public NPinPart<1> {
private:
	scalar voltage;
	size_t branch_id;

	scalar current;

public:
	explicit VoltageSource(const std::string &name, scalar voltage);
	~VoltageSource() noexcept;

	size_t num_needed_matrix_rows() const override { return pin().node->is_ground ? 0 : 1; }
	void set_first_matrix_row_id(size_t row_id) override { branch_id = row_id; }

	std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(scalar timestep) const override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	void update_value_from_result(size_t i, scalar value) override;
};


class VoltageSource2Pin : public NPinPart<2> {
private:
	scalar voltage;
	size_t branch_id;

	scalar current;

public:
	explicit VoltageSource2Pin(const std::string &name, scalar voltage);
	~VoltageSource2Pin() noexcept;

	size_t num_needed_matrix_rows() const override { return 1; }
	void set_first_matrix_row_id(size_t row_id) override { branch_id = row_id; }

	std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(scalar timestep) const override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	void update_value_from_result(size_t i, scalar value) override;
};