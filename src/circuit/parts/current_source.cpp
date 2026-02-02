#include "circuit/parts/current_source.h"

#include "circuit/n_pin_part.h"
#include "circuit/part.h"
#include "circuit/pin.h"
#include "circuit/scalar.h"

#include <string>


CurrentSource::CurrentSource(const std::string &name, scalar current) :
	NPinPart<2>(name),
	current(current) {
}

void CurrentSource::stamp_rhs_entries(std::vector<scalar> &rhs, [[maybe_unused]] const StampParams &params) {
	const auto &node0 = pin(0).node;
	const auto &node1 = pin(1).node;

	if (!node0->is_ground) rhs[node0->node_id] = -current;
	if (!node1->is_ground) rhs[node1->node_id] = current;
}

scalar CurrentSource::get_current_between([[maybe_unused]] const ConstPin &a, [[maybe_unused]] const ConstPin &b) const {
	return current;
}

