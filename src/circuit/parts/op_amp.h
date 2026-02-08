#pragma once

#include "circuit/n_pin_part.h"
#include "circuit/part.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <string>


// Operational amplifier with hysteresis
class OpAmp : public NPinPart<3> {
private:
	enum class Mode {
		Linear,
		SatHigh,
		SatLow
	};

	scalar v_min;
	scalar v_max;

	scalar amplification;

	size_t branch_id;
	Mode mode;

	constexpr static std::array<std::string_view, 3> pin_names = { "plus", "minus", "out" };

	constexpr static scalar hysteresis = 1e-3_s;

public:
	enum class Pins {
		Plus,
		Minus,
		Out
	};

	using NPinPart<3>::pin;

	Pin pin(const Pins &p) {
		return pin(static_cast<size_t>(p));
	}

	ConstPin pin(const Pins &p) const {
		return pin(static_cast<size_t>(p));
	}

	std::string_view get_pin_name(size_t pin_id) const noexcept override {
		return pin_names[pin_id];
	}

	OpAmp(const std::string &name, scalar v_min, scalar v_max, scalar amplification);
	~OpAmp() noexcept = default;

	size_t num_needed_matrix_rows() const override { return 1; }
	void set_first_matrix_row_id(size_t row_id) override { branch_id = row_id; }
	size_t get_first_matrix_row_id() override { return branch_id; }

	std::vector<MatrixEntry> gen_matrix_entries(const StampParams &params) override;
	void stamp_rhs_entries([[maybe_unused]] std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) override;

	void update(const StampParams &params) override;

	scalar get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const override { return 0.0; }
};