#include "leptValue.h"
#include "leptParser.h"
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
}
