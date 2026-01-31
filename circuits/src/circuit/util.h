#pragma once

#include "scalar.h"
#include <numbers>
#include <string>


std::string make_timestamp();

size_t floor_sqrt(size_t n);
size_t ceil_sqrt(size_t n);

constexpr scalar tau = 2.0 * std::numbers::pi_v<scalar>;