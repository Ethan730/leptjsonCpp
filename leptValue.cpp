#include "leptValue.h"
#include "leptParser.h"
#include <cassert>
namespace lept_json {
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
		this->n = n;
	}

	
}
