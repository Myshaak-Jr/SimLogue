#include "./node.h"


Node::Node() : row_id(0), ground(false) {}

size_t Node::get_row_id() const { return row_id; }
void Node::set_row_id(size_t row_id) { this->row_id = row_id; }

void Node::set_ground() { ground = true; }
bool Node::is_ground() const { return ground; }