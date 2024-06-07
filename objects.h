#pragma once
#include <string>

class StringObject {
public:
	std::string string;
	uint32_t hash;

	StringObject(std::string string) {
		this->string = string;
		this->hash = hashString(this->string.c_str(), this->string.size());
	}

	std::string getString() {
		return this->string;
	}

	uint32_t getHash() {
		return this->hash;
	}

	uint32_t hashString(const char* key, int length) {
		uint32_t hash = 2166136261u;
		for (int i = 0; i < length; i++) {
			hash ^= (uint8_t)key[i];
			hash *= 16777619;
		}
		return hash;
	}

};

