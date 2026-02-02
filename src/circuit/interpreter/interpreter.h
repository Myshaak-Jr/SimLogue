#pragma once

#include "circuit/circuit.h"
#include "circuit/interpreter/quantity.h"
#include "circuit/part.h"

#include <format>
#include <istream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>


class ParseError : public std::runtime_error {
public:
	explicit ParseError(const std::string &message)
		: std::runtime_error(message) {}
};

class Interpreter {
private:
	Circuit &circuit;

	bool parsing_comment;

	std::unordered_map<std::string, Part *> parts;

	static bool check_name(const std::string &name);

	struct Value {
		Quantity quantity = Quantity::Voltage;
		scalar value = 0.0;
	};

	Part *parse_part(const std::string &partname, size_t line_idx) const;
	Pin parse_pin(const std::string &pinname, size_t line_idx, bool support_twopin = false, size_t twopin_part_pin_id = 0) const;
	void parse_connections(const std::vector<std::string_view> &tokens, size_t line_idx) const;

	std::vector<std::string_view> tokenize(std::string_view line);

	// parses and executes a line
	void execute_line(std::string_view line, size_t line_idx);

	struct ParamInfo {
		Quantity quantity;
		bool has_default_value;
		scalar default_value;

		ParamInfo(Quantity quantity) : quantity(quantity), has_default_value(false), default_value(0.0) {}
		ParamInfo(Quantity quantity, scalar default_value) : quantity(quantity), has_default_value(true), default_value(default_value) {}
	};

	// supports all parts with the first constructor argument being string, and then only scalars
	template <class T, size_t N>
	void add_basic_part(const std::vector<std::string_view> &tokens, size_t &curr_token, size_t line_idx, const std::string &part_type_name, const std::array<ParamInfo, N> &constructor_signature);

	template <class T>
	void add_basic_part(const std::vector<std::string_view> &tokens, size_t &curr_token, size_t line_idx, const std::string &part_type_name) {
		add_basic_part<T, 0>(tokens, curr_token, line_idx, part_type_name, std::array<ParamInfo, 0>());
	}

	friend class Circuit;
	Interpreter(Circuit &circuit);

	static Value parse_value(std::string_view value_string, size_t line_idx) {
		return parse_value(value_string, std::format("on line {}", line_idx));
	}

public:
	static Value parse_value(std::string_view value_string, std::string_view where);

	void execute(std::istream &in);
	void execute(const std::string &script);
};



template <class T, size_t N>
void Interpreter::add_basic_part(const std::vector<std::string_view> &tokens, size_t &curr_token, size_t line_idx, const std::string &part_type_name, const std::array<ParamInfo, N> &constructor_signature) {
	if (++curr_token >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected part name after '{}', got ''", line_idx, part_type_name));
	std::string partname(tokens[curr_token]);

	if (!check_name(partname)) throw ParseError(std::format("Name error on line {}: Invalid part name '{}'.", line_idx, partname));
	if (parts.find(partname) != parts.end()) throw ParseError(std::format("Syntax error on line {}: Redefinition of part name '{}'.", line_idx, partname));

	Part *part = nullptr;

	// parse the constructor values
	std::array<Value, N> parsed_values{};
	parsed_values.fill(Value{});

	size_t values_size = 0;

	bool first_param = true;

	for (++curr_token; curr_token < tokens.size(); curr_token += 2) {
		auto separator = tokens[curr_token];
		if (curr_token + 1 >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Invalid number of parameters for {} {}.", line_idx, part_type_name, partname));
		auto value_string = tokens[curr_token + 1];

		std::string_view expected_separator;
		if (first_param) {
			expected_separator = ":";
			first_param = false;
		}
		else {
			expected_separator = ",";
		}

		if (separator != expected_separator) {
			throw ParseError(std::format("Syntax error on line {}: Expected '{}' before '{}', got {}.", line_idx, expected_separator, value_string, separator));
		}

		if (values_size >= N) {
			throw ParseError(std::format("Syntax error on line {}: Invalid number of parameters for {} {}.", line_idx, part_type_name, partname));
		}
		parsed_values[values_size] = parse_value(value_string, line_idx);
		++values_size;
	}

	std::array<bool, N> used_values{};
	used_values.fill(false);
	std::array<scalar, N> params{};
	params.fill(scalar{ 0 });

	for (size_t i = 0; i < constructor_signature.size(); ++i) {
		auto &[search_quantity, has_default, def_val] = constructor_signature[i];

		bool found = false;

		for (size_t j = 0; j < values_size; ++j) {
			if (used_values[j]) continue;

			auto &[quantity, value] = parsed_values[j];

			if (quantity != search_quantity) continue;

			params[i] = value;

			found = true;
			used_values[j] = true;
			break;
		}

		if (found) continue;

		if (has_default) {
			params[i] = def_val;
			continue;
		}

		throw ParseError(std::format("Parameter error on line {}: Unable to find value for parameter {} ({}).", line_idx, i, quantity_to_string(search_quantity)));
	}

	part = std::apply([&](auto... params) { return circuit.add_part<T>(partname, params...); }, params);

	parts[partname] = part;
}
