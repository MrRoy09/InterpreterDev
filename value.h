#pragma once
#include <variant>
#include <iostream>
#include <stdexcept>
#include "objects.h"

class Value {
public:
	bool isNill;
	std::variant<bool, double, StringObject> value;

	Value(bool value) {
		this->isNill = 0;
		this->value = value;
	}

	Value(double value) {
		this->isNill = 0;
		this->value = value;
	}

	Value(std::string string) {
		this->isNill = 0;
		this->value = StringObject(string);
	}

	Value() {
		this->isNill = 0;
	}

	void printValue() {
		if (std::holds_alternative<bool>(value)) {
			std::cout<<std::get<bool>(value)<<"\n";
		}
		else if (std::holds_alternative<double>(value)) {
			std::cout<<std::get<double>(value) << "\n";
		}
		else if (std::holds_alternative<StringObject>(value)) {
			std::cout <<std::get<StringObject>(value).getString() << "\n";
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
		if (this->value.index() != b.value.index()) return false;

		else if (std::holds_alternative<bool>(value)){
			return this->returnBool() == b.returnBool();
		}

		else if (std::holds_alternative<double>(value)) {
			return this->returnDouble() == b.returnDouble();
		}

		else if (this->isNill) {
			return true;
		}

		else if (std::holds_alternative<StringObject>(value)) {
			return this->returnString() == b.returnString();
		}
	}
};



