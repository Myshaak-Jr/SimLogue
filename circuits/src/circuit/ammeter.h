#pragma once


class Ammeter {
private:
	size_t row_id;

public:
	Ammeter();

	void set_row_id(size_t row_id);
	size_t get_row_id() const;

	double get_last_current() const;
};