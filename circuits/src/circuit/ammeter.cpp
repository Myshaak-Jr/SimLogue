#include <cstddef>

#include "ammeter.h"


Ammeter::Ammeter() : row_id(0) {}


void Ammeter::set_row_id(size_t id) { row_id = id; }


size_t Ammeter::get_row_id() const { return row_id; }