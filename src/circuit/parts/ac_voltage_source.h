#pragma once


#include "circuit/n_pin_part.h"
#include "circuit/part.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <string>


class AcVoltageSource : public NPinPart<1> {
private:
	scalar angular_vel;
	scalar amplitude;
	scalar phase;

	scalar voltage;

	size_t branch_id;
	scalar current;

public:
	AcVoltageSource(const std::string &name, scalar frequency, scalar amplitude, scalar phase = 0.0);
	~AcVoltageSource() noexcept;

	size_t num_needed_matrix_rows() const override { return pin().node->is_ground ? 0 : 1; }
	void set_first_matrix_row_id(size_t row_id) override { branch_id = row_id; }
	size_t get_first_matrix_row_id() override { return branch_id; }

	std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(const StampParams &params) override;
	void stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	void update_value_from_result(size_t i, scalar value) override;
	void update(const StampParams &params) override;
};


class AcVoltageSource2Pin : public NPinPart<2> {
private:
	scalar angular_vel;
	scalar amplitude;
	scalar phase;

	scalar voltage;
	size_t branch_id;

	scalar current;

public:
	AcVoltageSource2Pin(const std::string &name, scalar frequency, scalar amplitude, scalar phase = 0.0);
	~AcVoltageSource2Pin() noexcept;

	size_t num_needed_matrix_rows() const override { return 1; }
	void set_first_matrix_row_id(size_t row_id) override { branch_id = row_id; }
	size_t get_first_matrix_row_id() override { return branch_id; }

	std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(const StampParams &params) override;
	void stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	void update_value_from_result(size_t i, scalar value) override;

	void update(const StampParams &params) override;
};