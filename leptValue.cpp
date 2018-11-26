#include "leptValue.h"
#include "leptParser.h"
#include <cassert>
namespace lept_json {
	Value::Value() :type(TYPE_NULL) {}

	Value::~Value() {
		unionFree();
	}

	Value::Value(const Value &v) {
		switch (v.type)
		{
		case TYPE_NUMBER:
			this->set_number(v.get_number());
			break;
		case TYPE_STRING:
			this->set_string(v.s);
			break;
		case TYPE_ARRAY:
			this->set_array(v.a);
			break;
		case TYPE_OBJECT:
			this->set_object(v.o);
			break;
		default:
			this->set_literal_type(v.type);
			break;
		}
	}

	Value& Value::operator=(const Value& v)
	{
		switch (v.type)
		{
		case TYPE_NULL:
		case TYPE_TRUE:
		case TYPE_FALSE:
			this->set_literal_type(v.type);
			break;
		case TYPE_NUMBER:
			this->set_number(v.n);
			break;
		case TYPE_STRING:
			this->set_string(v.s);
			break;
		case TYPE_ARRAY:
			this->set_array(v.a);
			break;
		case TYPE_OBJECT:
			this->set_object(v.o);
			break;
		}
		return *this;
	}

	void Value::parse(const std::string & json) {
		Parser(*this, json);
	}

	value_type Value::get_type() const noexcept {
		return this->type;
	}

	void Value::set_literal_type(value_type type) noexcept {
		assert(type == TYPE_NULL || type == TYPE_TRUE || type == TYPE_FALSE);
		unionFree();
		this->type = type;
	}

	double Value::get_number() const noexcept {
		assert(this->type == TYPE_NUMBER);
		return this->n;
	}

	void Value::set_number(double n) noexcept {
		unionFree();
		this->type = TYPE_NUMBER;
		this->n = n;
	}

	std::string Value::get_string() const noexcept
	{
		assert(this->type == TYPE_STRING);
		return this->s;
	}

	void Value::set_string(const std::string& s) noexcept {
		if (this->type == TYPE_STRING) {
			this->s = s;
		}
		else {
			unionFree();
			this->type = TYPE_STRING;
			new(&this->s) std::string(s);
		}	
	}

	std::size_t Value::get_array_size() const noexcept
	{
		assert(this->type == TYPE_ARRAY);
		return this->a.size();
	}

	const Value& Value::get_array_element(std::size_t index) const noexcept
	{
		assert(this->type == TYPE_ARRAY);
		assert(index < this->a.size());
		return a[index];
	}

	void Value::set_array(const std::vector<Value>& a) noexcept
	{
		if (this->type == TYPE_ARRAY) {
			this->a = a;
		}
		else {
			unionFree();
			this->type = TYPE_ARRAY;
			new(&this->a) std::vector<Value>(a);
		}
	}

	std::size_t Value::get_object_size() const noexcept
	{
		assert(this->type == TYPE_OBJECT);
		return this->o.size();
	}

	std::string Value::get_object_key(std::size_t index) const noexcept
	{
		assert(this->type == TYPE_OBJECT);
		assert(index < this->o.size());
		return this->o[index].get_key();
	}

	const Value& Value::get_object_value(std::size_t index) const noexcept
	{
		assert(this->type == TYPE_OBJECT);
		assert(index < this->o.size());
		return this->o[index].get_value();
	}

	void Value::set_object(const std::vector<Member>& o) noexcept
	{
		if (this->type == TYPE_OBJECT)
			this->o = o;
		else {
			unionFree();
			this->type = TYPE_OBJECT;
			new(&this->o) std::vector<Member>(o);
		}
	}

	void Value::unionFree() noexcept {
		switch (this->type)
		{
		case TYPE_STRING:
			s.std::string::~string();
			break;
		case TYPE_ARRAY:
			a.std::vector<Value>::~vector();
		case TYPE_OBJECT:
			o.std::vector<Member>::~vector();
		default:
			break;
		}
		this->type = TYPE_NULL;
	}

	std::string Member::get_key() const noexcept
	{
		return this->key;
	}

	void Member::set_key(const std::string& key) noexcept
	{
		this->key = key;
	}

	const Value & Member::get_value() const noexcept
	{
		return this->v;
	}

	void Member::set_value(const Value &v) noexcept
	{
		this->v = v;
	}
}
