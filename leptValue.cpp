#include "leptValue.h"
#include "leptParser.h"
#include <cassert>
#include <cstdio>
namespace jsonCpp {
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

	std::string Value::stringify_string(const std::string& s) const noexcept {
		std::string res="\"";
		for (unsigned char c : s) {
			switch (c)
			{
			case '\"':res += "\\\""; break;
			case '\\':res += "\\\\"; break;
			case '/':res += "\\/"; break;
			case '\b':res += "\\b"; break;
			case '\f':res += "\\f"; break;
			case '\n':res += "\\n"; break;
			case '\r':res += "\\r"; break;
			case '\t':res += "\\t"; break;
			default:
				if (c < 0x20) {
					char buffer[7];
					sprintf(buffer, "\\u%04X", c);
					res += buffer;
				}
				else
					res += c;
				break;
			}
		}
		res += '"';
		return res;
	}

	std::string Value::stringify_value() const noexcept
	{
		std::string res;
		switch (this->type)
		{
		case TYPE_NULL: res = "null"; break;
		case TYPE_TRUE:res = "true"; break;
		case TYPE_FALSE:res = "false"; break;
		case TYPE_NUMBER:
			char num[32];
			sprintf(num, "%.17g", this->n);
			res = num;
			break;
		case TYPE_STRING:res = stringify_string(this->s); break;
		case TYPE_ARRAY:
			res = "[";
			for (std::size_t i = 0; i < this->a.size(); ++i) {
				if (i > 0)
					res += ",";
				res += this->a[i].stringify();
			}
			res += "]";
			break;
		case TYPE_OBJECT:
			res = "{";
			for (std::size_t i = 0; i < this->o.size(); ++i) {
				if (i > 0)
					res += ",";
				res += stringify_string(this->o[i].get_key());
				res += ":";
				res += this->o[i].get_value().stringify_value();
			}
			res += "}";
			break;
		}
		return res;
	}

	std::string Value::stringify() const noexcept
	{
		return stringify_value();
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
			break;
		case TYPE_OBJECT:
			o.std::vector<Member>::~vector();
			break;
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

	bool operator==(const Value & v1, const Value & v2)
	{
		if (v1.type != v2.type) return false;
		switch (v1.type)
		{
		case TYPE_NUMBER:
			return v1.n == v2.n;
		case TYPE_STRING:
			return v1.s == v2.s;
		case TYPE_ARRAY:
			if (v1.a.size() != v2.a.size()) return false;
			for (std::size_t i = 0; i < v1.a.size(); ++i) {
				if (v1.a[i] != v2.a[i])
					return false;
			}
			return true;
		case TYPE_OBJECT:
			if (v1.o.size() != v2.o.size()) return false;
			for (std::size_t i = 0; i < v1.o.size(); ++i) {
				const Value* tmp = v2.find_object_value(v1.o[i].get_key());
				if (!tmp)
					return false;
				if (*tmp != v1.o[i].get_value())
					return false;
			}
			return true;
		default:
			return true;
		}
	}

	bool operator!=(const Value & v1, const Value & v2)
	{
		return !operator==(v1, v2);
	}

	const Value* Value::find_object_value(const std::string &key) const noexcept
	{
		for (std::size_t i = 0; i < this->o.size(); ++i) {
			if (this->o[i].get_key() == key)
				return &this->o[i].get_value();
		}
		return nullptr;
	}
}
