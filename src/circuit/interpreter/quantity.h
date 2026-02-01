#pragma once

#include "circuit/scalar.h"
#include "circuit/util.h"

#include <array>
#include <numbers>
#include <optional>
#include <string>


enum class Quantity {
	Current,
	Voltage,
	Resistance,
	Capacitance,
	Inductance,
	Time,
	Frequency,
	Angle
};

struct UnitInfo {
	Quantity quantity;
	scalar ratio_to_base;
};

struct UnitEntry {
	std::string_view unit;
	UnitInfo info;
};


constexpr std::array<UnitEntry, 14> unit_table{ {
		// Current
		{ "A",    { Quantity::Current, 1.0 } },
		{ "Am",   { Quantity::Current, 1.0 } },

		// Voltage
		{ "V",    { Quantity::Voltage, 1.0 } },

		// Resistance
		{ "Ohm",  { Quantity::Resistance, 1.0 } },
		{ "Ω",    { Quantity::Resistance, 1.0 } },

		// Capacitance
		{ "F",    { Quantity::Capacitance, 1.0 } },

		// Inductance
		{ "H",    { Quantity::Inductance, 1.0 } },

		// Time
		{ "s",    { Quantity::Time, 1.0 } },
		{ "min",  { Quantity::Time, 60.0 } },

		// Frequency
		{ "Hz",   { Quantity::Frequency, 1.0 } },

		// Angle (base = rad)
		{ "rad",  { Quantity::Angle, 1.0 } },
		{ "deg",  { Quantity::Angle, tau / 360.0 } },
		{ "°",    { Quantity::Angle, tau / 360.0 } },
		{ "grad", { Quantity::Angle, tau / 400.0 } },
	} };


[[nodiscard]] constexpr std::string_view quantity_to_unit(const Quantity &quantity) noexcept {
	using enum Quantity;

	switch (quantity) {
		case Current:     return "A";
		case Voltage:     return "V";
		case Resistance:  return "Ohm";
		case Capacitance: return "F";
		case Inductance:  return "H";
		case Time:        return "s";
		case Frequency:   return "Hz";
		case Angle:       return "rad";
	}

	return "?";
}

[[nodiscard]] constexpr std::optional<UnitInfo> unit_to_quantity(std::string_view unit) noexcept {
	for (const auto &entry : unit_table) {
		if (entry.unit == unit) return entry.info;
	}

	return std::nullopt;
}

[[nodiscard]] constexpr std::string_view quantity_to_string(const Quantity &quantity) noexcept {
	using enum Quantity;

	switch (quantity) {
		case Current:     return "current";
		case Voltage:     return "voltage";
		case Resistance:  return "resistance";
		case Capacitance: return "capacitance";
		case Inductance:  return "inductance";
		case Time:        return "time";
		case Frequency:   return "frequency";
		case Angle:       return "angle";
	}

	return "?";
}