#include "./part_base.h"

#include "./node.h"
#include "./ammeter.h"


Part::Part() : ammeters() {}

void Part::measure_current(Pin a, Pin b, Ammeter* am) {
	
}

const std::vector<Ammeter*>& Part::get_measured_pins() const {
	return ammeters;
}