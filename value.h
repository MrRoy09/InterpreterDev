#pragma once
#include <variant>
#include <iostream>
#include <stdexcept>
#include "objects.h"

class Value {
public:
	const char* type;
	std::variant<bool, double, StringObject> value;

	Value(const char* type, bool value) {
		this->type = type;
		this->value = value;
	}

	Value(const char* type, double value) {
		this->type = type;
		this->value = value;
	}

	Value(std::string string) {
		this->type = "string";
		this->value = StringObject(string);
	}


	Value(const char* type) {
		double nil = 0;
		this->type = "nil";
		this->value = nil;
	}

	Value() {
		this->type = "nil";
	}

	void printValue() {
		if (std::holds_alternative<bool>(value)) {
			std::cout << type<<":" << std::get<bool>(value) << "\n";
		}
		else if (std::holds_alternative<double>(value)) {
			std::cout <<type<<":"<< std::get<double>(value) << "\n";
		}
		else if (std::holds_alternative<StringObject>(value)) {
			std::cout << type << ":" << std::get<StringObject>(value).getString() << "\n";
		}
	}

	bool returnBool() {
		if (std::holds_alternative<bool>(value)) {
			return std::get<bool>(value);
		}
		else if (std::holds_alternative<double>(value)) {
			return std::get<double>(value);
		}
		else {
			throw std::bad_variant_access();
		}
	}

	double returnDouble() {
		if (std::holds_alternative<double>(value)) {
			return std::get <double>(value);
		}
		else {
			throw std::bad_variant_access();
		}
	}

	std::string returnString() {
		if (std::holds_alternative<StringObject>(value)) {
			return std::get <StringObject>(value).getString();
		}
		else {
			throw std::bad_variant_access();
		}
	}

	uint32_t returnHash() {
		if (std::holds_alternative<StringObject>(value)) {
			return std::get<StringObject>(value).getHash();
		}
		else {
			throw std::bad_variant_access();
		}
	}

	StringObject returnStringObject() {
		if (std::holds_alternative<StringObject>(value)) {
			return std::get<StringObject>(value);
		}
	}

	bool ValuesEqual(Value b) {
		if (this->type != b.type) return false;

		else if (this->type == "bool") {
			return this->returnBool() == b.returnBool();
		}

		else if (this->type == "double") {
			return this->returnDouble() == b.returnDouble();
		}

		else if (this->type == "nil") {
			return true;
		}

		else if (this->type == "string") {
			return this->returnString() == b.returnString();
		}
	}
};



