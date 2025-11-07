#include "./part_base.h"

#include "./node.h"
#include "./ammeter.h"


Part::Part() : ammeter(nullptr) {}


bool Part::has_ammeter() const {
	return ammeter != nullptr;
}

Ammeter* Part::get_ammeter() {
	return ammeter;
}

const Ammeter* Part::get_ammeter() const {
	return ammeter;
}

void Part::set_ammeter(Ammeter* ammeter) {
	this->ammeter = ammeter;
}