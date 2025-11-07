#pragma once

#include <vector>

#include "part_base.h"


class Node {
private:
	size_t row_id;
	bool ground;

public:
	Node();
	~Node() = default;

	void set_row_id(size_t row_id);
	size_t get_row_id() const;

	void set_ground();
	bool is_ground() const;
};
