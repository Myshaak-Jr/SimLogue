#pragma once

#include "circuit/n_pin_part.h"
#include "circuit/part.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <queue>
#include <string>
#include <vector>


class Switch : public NPinPart<2> {
private:
	const scalar off_resistance = 10_M;
	size_t branch_id;
	scalar last_i;

	bool on;

	enum class EventType {
		ON,
		OFF
	};

	struct Event {
		size_t step;
		EventType type;
	};

	struct Comp {
		bool operator()(const Event &a, const Event &b) const noexcept {
			if (a.step != b.step) return a.step > b.step;
			return a.type == EventType::OFF && b.type == EventType::ON;
		}
	};

	std::priority_queue<Event, std::vector<Event>, Comp> events;

public:
	Switch(const std::string &name, bool on = false);
	~Switch() noexcept = default;

	std::vector<std::tuple<size_t, size_t, scalar>> gen_matrix_entries(const StampParams &params) override;
	void stamp_rhs_entries([[maybe_unused]] std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) override {}

	void update(const StampParams &params) override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	void switch_on() { on = false; }
	void switch_off() { on = true; }

	void schedule_on(size_t step);
	void schedule_off(size_t step);

	size_t num_needed_matrix_rows() const override { return 1; }
	void set_first_matrix_row_id(size_t row_id) override { branch_id = row_id; }
	size_t get_first_matrix_row_id() override { return branch_id; }

	void update_value_from_result([[maybe_unused]] size_t i, scalar value) override { last_i = value; }
};