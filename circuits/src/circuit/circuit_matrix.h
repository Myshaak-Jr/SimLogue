#pragma once

#include <vector>

#include "./node.h"

#include "./ammeter.h"


class CircuitMatrix {
private:
	


	std::vector<double> rhs;

	size_t num_rows;

public:
	CircuitMatrix();

	void clear();

	size_t reserve_row();

	void prepare_matrix();
	
	void stamp_conductance(size_t row_1, size_t row_2, double conductance);
	void stamp_rhs(size_t row, double value);
	void stamp_voltage_source(size_t row, double voltage);
};

