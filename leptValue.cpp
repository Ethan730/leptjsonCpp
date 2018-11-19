#include "leptValue.h"
#include "leptParser.h"
#include <cassert>
namespace lept_json {
	Value::Value() :type(TYPE_NULL) {}
	Value::~Value() {
		free();
	}
	void Value::parse(const std::string & json) {
		Parser(*this, json);
	}

	value_type Value::get_type() const noexcept {
		return this->type;
	}

	void Value::set_type(value_type type) noexcept {
		this->type = type;
	}

	double Value::get_number() noexcept {
		assert(this->type == TYPE_NUMBER);
		return this->n;
	}

	void Value::set_number(double n) noexcept {
		free();
		this->type = TYPE_NUMBER;
		this->n = n;
	}

	std::string Value::get_string() noexcept
	{
		assert(this->type == TYPE_STRING);
		return this->s;
	}

	void Value::set_string(const std::string& s) noexcept {
		if (this->type == TYPE_STRING) {
			this->s = s;
		}
		else {
			free();
			this->type = TYPE_STRING;
			new(&this->s) std::string(s);
		}	
	}
	
	void Value::free() noexcept {
		switch (this->type)
		{
		case TYPE_STRING:
			s.std::string::~string();
			break;
		default:
			break;
		}
		this->type = TYPE_NULL;
	}
}
