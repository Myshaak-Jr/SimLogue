#pragma once

#include "scalar.h"

#include <cstddef>


struct Node {
	scalar voltage = 0.0;
	size_t node_id = 0;
	bool is_ground = false;
};
