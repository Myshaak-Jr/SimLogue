#include "circuit/interpreter/interpreter.h"

#include "circuit/parts/ac_voltage_source.h"
#include "circuit/parts/capacitor.h"
#include "circuit/parts/current_source.h"
#include "circuit/parts/inductor.h"
#include "circuit/parts/op_amp.h"
#include "circuit/parts/resistor.h"
#include "circuit/parts/switch.h"
#include "circuit/parts/voltage_source.h"
#include "circuit/util.h"

#include <iostream>
#include <sstream>
#include <tuple>


Interpreter::Interpreter(Circuit &circuit) : circuit(circuit), parsing_comment(false) {
	parts["GND"] = circuit.get_ground();
}

static bool is_first_word_letter(char x) {
	return (x == '_') || ('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z');
}

static bool is_digit(char x) {
	return ('0' <= x && x <= '9');
}

static bool is_word_letter(char x) {
	return is_first_word_letter(x) || is_digit(x);
}

bool Interpreter::check_name(const std::string &name) {
	if (name.empty() || !is_first_word_letter(name[0])) return false;
	for (size_t i = 1; i < name.size(); ++i) {
		if (!is_word_letter(name[i])) return false;
	}
	return true;
}

Part *Interpreter::parse_part(const std::string &partname, size_t line_idx) const {
	if (!check_name(partname)) throw ParseError(std::format("Name error on line {}: Invalid part name '{}'.", line_idx, partname));

	auto it = parts.find(partname);
	if (it == parts.end()) throw ParseError(std::format("Name error on line {}: Unknown part name '{}'.", line_idx, partname));

	return it->second;
}

Pin Interpreter::parse_pin(const std::string &pinname, size_t line_idx, bool support_twopin, size_t twopin_part_pin_id) const {
	int dot_pos = -1;
	for (size_t i = 0; i < pinname.size(); ++i) {
		if (pinname[i] == '.') dot_pos = static_cast<int>(i);
	}

	if (dot_pos == -1) {
		auto part = parse_part(pinname, line_idx);

		if (part->pin_count() == 1) {
			return part->pin(0);
		}
		else if (part->pin_count() == 2 && support_twopin) {
			return part->pin(twopin_part_pin_id);
		}
		else {
			throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));
		}
	}

	if (dot_pos == 0 || static_cast<size_t>(dot_pos) == pinname.size() - 1) {
		throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));
	}

	std::string partname = pinname.substr(0, dot_pos);
	std::string pin = pinname.substr(dot_pos + 1, pinname.size() - (dot_pos + 1));

	if (!check_name(pin)) throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));

	auto part = parse_part(partname, line_idx);

	try {
		Pin p = part->pin(pin);
		return p;
	}
	catch (const std::out_of_range &) {
		throw ParseError(std::format("Name error on line {}: {} doesn't have pin {}.", line_idx, partname, pin));
	}
}


Interpreter::Value Interpreter::parse_value(std::string_view value_string, std::string_view where) {
	std::string number_string = "";
	number_string.reserve(value_string.size());

	size_t i = 0;
	for (; i < value_string.size(); ++i) {
		auto letter = value_string[i];
		if (letter == '_') continue;
		if (!is_digit(letter) && letter != '.') break;
		number_string.push_back(letter);
	}

	scalar value;
	auto [ptr, ec] = std::from_chars(number_string.data(), number_string.data() + number_string.size(), value);
	if (ec != std::errc()) {
		throw ParseError(std::format("Syntax error {}: Invalid number '{}'.", where, number_string));
	}

	auto unit_token = value_string.substr(i);

	// if there is something like 256_m_V, the first underscore is already deleted by the number detection,
	// so this will take into account the second one
	size_t underscore_pos = 0;
	for (; underscore_pos < unit_token.size(); ++underscore_pos) {
		if (unit_token[underscore_pos] == '_') break;
	}

	std::string_view unit_mult;
	std::string_view unit_string;

	if (underscore_pos == unit_token.size() - 1) {
		throw ParseError(std::format("Syntax error {}: Invalid unit '{}'.", where, unit_token));
	}

	if (underscore_pos == unit_token.size()) {
		auto [quantity, ratio] = unit_to_quantity(unit_token);
		if (quantity != Quantity::Unknown) {
			value *= ratio;

			// this way there will be no multiplier, so we can safely return the value
			return { quantity, value };
		}

		// the unit as a whole isn't valid, so we will try using the first letter as a multiplier

		// I hate encodings
		if (unit_token.starts_with("μ")) {
			unit_mult = unit_token.substr(0, 2);
			unit_string = unit_token.substr(2);
		}
		else {
			unit_mult = unit_token.substr(0, 1);
			unit_string = unit_token.substr(1);
		}
	}
	else {
		unit_mult = unit_token.substr(0, underscore_pos);
		unit_string = unit_token.substr(underscore_pos + 1);
	}

	auto [quantity, ratio] = unit_to_quantity(unit_string);
	if (quantity == Quantity::Unknown || (quantity == Quantity::None && unit_mult != "")) {
		throw ParseError(std::format("Syntax error {}: Invalid unit '{}'.", where, unit_token));
	}

	value *= ratio;


	if (unit_mult == "E") value *= 1e18;
	else if (unit_mult == "P") value *= 1e15;
	else if (unit_mult == "T") value *= 1e12;
	else if (unit_mult == "G") value *= 1e9;
	else if (unit_mult == "M") value *= 1e6;
	else if (unit_mult == "k") value *= 1e3;
	else if (unit_mult == "");
	else if (unit_mult == "m") value *= 1e-3;
	else if (unit_mult == "u") value *= 1e-6;
	else if (unit_mult == "μ") value *= 1e-6;
	else if (unit_mult == "n") value *= 1e-9;
	else if (unit_mult == "p") value *= 1e-12;
	else if (unit_mult == "f") value *= 1e-15;
	else if (unit_mult == "a") value *= 1e-18;
	else throw ParseError(std::format("Syntax error {}: Invalid unit '{}'.", where, unit_token));

	return { quantity, value };
}

std::vector<std::string_view> Interpreter::tokenize(std::string_view line) {
	// tokenize with white space as token separator
	// ':', '-' and ',' are always treated as self-contained tokens
	std::vector<std::string_view> tokens;

	size_t token_lo = 0;
	size_t token_hi = 0;

	while (true) {
		if (parsing_comment) {
			if (line.substr(0, token_hi + 1).ends_with("*/")) {
				parsing_comment = false;
				token_lo = token_hi + 1;
			}

			++token_hi;

			if (token_hi == line.size()) {
				return tokens;
			}

			continue;
		}

		if (line.substr(0, token_hi + 1).ends_with("//")) {
			auto token = line.substr(token_lo, token_hi - token_lo - 1);
			if (!token.empty()) tokens.push_back(token);

			return tokens;
		}

		if (token_hi == line.size()) {
			auto token = line.substr(token_lo, token_hi - token_lo);
			if (!token.empty()) tokens.push_back(token);

			return tokens;
		}

		auto letter = line[token_hi];

		if (line.substr(0, token_hi + 1).ends_with("/*")) {
			auto token = line.substr(token_lo, token_hi - token_lo - 1);
			if (!token.empty()) tokens.push_back(token);

			parsing_comment = true;
		}
		else if (std::isspace(letter)) {
			auto token = line.substr(token_lo, token_hi - token_lo);
			if (!token.empty()) tokens.push_back(token);

			token_lo = token_hi + 1;
		}
		else if (letter == ',' || letter == '-' || letter == ':') {
			auto token = line.substr(token_lo, token_hi - token_lo);
			if (!token.empty()) tokens.push_back(token);

			tokens.push_back(line.substr(token_hi, 1));
			token_lo = token_hi + 1;
		}

		++token_hi;
	}
}


void Interpreter::execute_line(std::string_view line, size_t line_idx) {
	using enum Quantity;

	std::vector<std::string_view> tokens = tokenize(line);

	if (tokens.empty()) return;

	size_t curr_token = 0;
	auto token = tokens[curr_token];

	// parts

	if (token == "capacitor") {
		add_basic_part<Capacitor>(
			tokens, curr_token, line_idx, "capacitor",
			std::array<ParamInfo, 1>{ Capacitance }
		);
	}
	else if (token == "current_source") {
		add_basic_part<CurrentSource>(
			tokens, curr_token, line_idx, "current_source",
			std::array<ParamInfo, 1>{ Current }
		);
	}
	else if (token == "inductor") {
		add_basic_part<Inductor>(
			tokens, curr_token, line_idx, "inductor",
			std::array<ParamInfo, 1>{ Inductance }
		);
	}
	else if (token == "resistor") {
		add_basic_part<Resistor>(
			tokens, curr_token, line_idx, "resistor",
			std::array<ParamInfo, 1>{ Resistance }
		);
	}
	else if (token == "switch") {
		add_basic_part<Switch>(
			tokens, curr_token, line_idx, "switch"
		);
	}
	else if (token == "voltage_source") {
		add_basic_part<VoltageSource>(
			tokens, curr_token, line_idx,
			"voltage_source",
			std::array<ParamInfo, 1>{ Voltage }
		);
	}
	else if (token == "voltage_source_2P") {
		add_basic_part<VoltageSource2Pin>(
			tokens, curr_token, line_idx, "voltage_source_2P",
			std::array<ParamInfo, 1>{ Voltage }
		);
	}
	else if (token == "ac_voltage_source") {
		add_basic_part<AcVoltageSource>(
			tokens, curr_token, line_idx, "ac_voltage_source",
			std::array<ParamInfo, 3>{ Frequency, Voltage, { Angle, 0.0_s } }
		);
	}
	else if (token == "ac_voltage_source_2P") {
		add_basic_part<AcVoltageSource2Pin>(
			tokens, curr_token, line_idx, "ac_voltage_source_2P",
			std::array<ParamInfo, 3>{ Frequency, Voltage, { Angle, 0.0_s } }
		);
	}
	else if (token == "op_amp") {
		add_basic_part<OpAmp>(
			tokens, curr_token, line_idx, "op_amp",
			std::array<ParamInfo, 3>{{ { Voltage, -12.0_s }, { Voltage, 12.0_s }, { None, 1e5_s } }}
		);
	}

	// other keywords

	else if (token == "scope") {
		if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected token 'current' or 'voltage' after 'scope', got ''", line_idx));

		auto scope_quantity = tokens[curr_token];

		bool is_current_scope = scope_quantity == "current";
		bool is_voltage_scope = scope_quantity == "voltage";

		if (is_current_scope || is_voltage_scope) {
			if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected token 'of' or 'between' after 'scope {}', got ''", line_idx, scope_quantity));

			auto scope_type = tokens[curr_token];

			if (scope_type != "of" && scope_type != "between") throw ParseError(std::format("Syntax error on line {}: Expected token 'of' or 'between' after 'scope {}', got '{}'", line_idx, scope_quantity, scope_type));

			if (scope_type == "of") {
				if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected part name after 'scope {} of', got ''", line_idx, scope_quantity));
				auto part = parse_part(std::string(tokens[curr_token]), line_idx);
				if (part->pin_count() != 2) throw ParseError(std::format("Syntax error on line {}: Expected a 2-pin part after 'scope {} of', got '{}'", line_idx, scope_quantity, tokens[curr_token]));

				if (is_current_scope) circuit.scope_current(part->pin(0), part->pin(1));
				else circuit.scope_voltage(part->pin(0), part->pin(1));
			}
			else if (scope_type == "between") {
				if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected pin name after 'scope {} between', got ''", line_idx, scope_quantity));
				auto pin_0 = parse_pin(std::string(tokens[curr_token]), line_idx);
				std::string_view names_and_keyword = "";
				if (++curr_token >= tokens.size() || (names_and_keyword = tokens[curr_token]) != "and") throw ParseError(std::format("Syntax error on line {}: Expected 'and' after 'scope {} between {}', got '{}'", line_idx, scope_quantity, pin_0.name, names_and_keyword));
				if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected pin name after 'scope {} between {} and', got ''", line_idx, scope_quantity, pin_0.name, names_and_keyword));
				auto pin_1 = parse_pin(std::string(tokens[curr_token]), line_idx);

				if (is_current_scope) circuit.scope_current(pin_0, pin_1);
				else circuit.scope_voltage(pin_0, pin_1);
			}
		}
		else {
			throw ParseError(std::format("Syntax error on line {}: Expected token 'current' or 'voltage' after 'scope', got '{}'", line_idx, scope_quantity));
		}
	}
	else if (token == "turn") {
		if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected token 'on' or 'off' after 'turn', got ''", line_idx));

		auto turn_to = tokens[curr_token];

		bool is_on = turn_to == "on";
		bool is_off = turn_to == "off";

		if (is_on || is_off) {
			if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected a switch name after 'turn {}', got ''", line_idx, turn_to));

			std::string switch_name(tokens[curr_token]);
			auto switch_part = dynamic_cast<Switch *>(parse_part(switch_name, line_idx));
			if (!switch_part) throw ParseError(std::format("Type error on line {}: {} is not a switch", line_idx, switch_name));

			std::string_view turn_at_keyword = "";
			if (++curr_token >= tokens.size() || (turn_at_keyword = tokens[curr_token]) != "at") throw ParseError(std::format("Syntax error on line {}: Expected token 'at' after 'turn {} {}', got '{}'", line_idx, turn_to, switch_name, turn_at_keyword));

			if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected a time value after 'turn {} {} at', got ''", line_idx, turn_to, switch_name));
			auto [q, t] = parse_value(tokens[curr_token], line_idx);

			if (q != Time) {
				throw ParseError(std::format("Value error on line {}: Expected a time value after 'turn {} {} at', got '{}'", line_idx, turn_to, switch_name, tokens[curr_token]));
			}

			if (is_on) {
				switch_part->schedule_on(static_cast<size_t>(t / circuit.get_timestep()));
			}
			else {
				switch_part->schedule_off(static_cast<size_t>(t / circuit.get_timestep()));
			}
		}
		else {
			throw ParseError(std::format("Syntax error on line {}: Expected token 'on' or 'off' after 'turn', got '{}'", line_idx, turn_to));
		}
	}
	else {
		parse_connections(tokens, line_idx);
		return;
	}

	++curr_token;

	if (curr_token < tokens.size()) {
		throw ParseError(std::format("Syntax error on line {}: Expected token '{}'", line_idx, tokens[curr_token]));
	}
}

void Interpreter::parse_connections(const std::vector<std::string_view> &tokens, size_t line_idx) const {
	for (size_t i = 0; i < tokens.size(); ++i) {
		auto pin_0 = parse_pin(std::string(tokens[i]), line_idx, true, 1);

		if (++i >= tokens.size()) break;

		auto pin_connector = tokens[i];
		if (pin_connector != "-") throw ParseError(std::format("Syntax error on line {}: Expected '-' after '{}', got '{}'", line_idx, tokens[i - 1], pin_connector));

		if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected a pin name after '{} -', got ''", line_idx, tokens[i - 2]));

		auto pin_1 = parse_pin(std::string(tokens[i]), line_idx, true, 0);

		circuit.connect(pin_0, pin_1);

		--i;
	}
}


void Interpreter::execute(std::istream &in) {
	std::string line;
	size_t i = 0;

	while (std::getline(in, line)) {
		if (line.empty()) { ++i; continue; }
		execute_line(line, i + 1);
		++i;
	}
}

void Interpreter::execute(const std::string &script) {
	std::istringstream iss(script);
	execute(iss);
}
