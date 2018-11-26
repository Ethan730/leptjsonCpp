#include "leptParser.h"
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include "leptException.h"
namespace lept_json {
	inline void expect(const char* &c, char ch) {
		assert(*c == ch);
		++c;
	}

	inline bool isdigit(char ch) {
		return ch <= '9'&&ch >= '0';
	}

	inline bool isdigit1to9(char ch) {
		return ch <= '9'&&ch >= '1';
	}

	Parser::Parser(Value & v, const std::string & json) :v(v), p(json.c_str()) {
		parse_whitespace();
		parse_value();
		parse_whitespace();
		if (*p != '\0') {
			v.set_literal_type(TYPE_NULL);
			throw jsonException("root not singular");	
		}
	}

	void Parser::parse_whitespace() noexcept {
		while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
			++p;
		}
	}

	void Parser::parse_literal(const std::string& literal, value_type type) {
		expect(p, literal[0]);
		size_t i;
		for(i=0;i<literal.size()-1;++i)
			if (p[i] != literal[i+1])
				throw jsonException("invalid value");
		v.set_literal_type(type);
		p += literal.size()-1;
	}

	void Parser::parse_number()
	{
		const char* tmp = p;
		if (*tmp == '-')
			++tmp;
		if (*tmp == '0')
			++tmp;
		else {
			if (!isdigit1to9(*tmp))
				throw jsonException("invalid value");
			while (isdigit(*tmp))
				++tmp;
		}
		if (*tmp == '.') {
			++tmp;
			if (!isdigit(*tmp))
				throw jsonException("invalid value");
			while (isdigit(*tmp))
				++tmp;
		}
		if (*tmp == 'e' || *tmp == 'E') {
			++tmp;
			if (*tmp == '+' || *tmp == '-')
				++tmp;
			if (!isdigit(*tmp))
				throw jsonException("invalid value");
			while (isdigit(*tmp))
				++tmp;
		}
		errno = 0;
		double n = strtod(p, nullptr);
		if (errno == ERANGE&&(n == HUGE_VAL || n == -HUGE_VAL))
			throw jsonException("number is too big");
		p = tmp;
		v.set_number(n);
	}

	bool Parser::parse_hex4(unsigned& u) noexcept
	{
		u = 0;
		for (int i = 0; i < 4; ++i) {
			char ch = *p++;
			u <<= 4;
			if (ch >= '0'&&ch <= '9') {
				u |= (ch - '0');
			}
			else if (ch >= 'a'&&ch <= 'f') {
				u |= (ch - 'a' + 10);
			}
			else if (ch >= 'A'&&ch <= 'F') {
				u |= (ch - 'A' + 10);
			}
			else
				return false;
		}
		return true;
	}

	std::string Parser::encode_utf8(unsigned u) noexcept
	{
		assert(u >= 0x0000 && u <= 0x10FFFF);
		std::string s;
		if (u <= 0x007F) {
			s += u;
		}
		else if (u <= 0x07FF) {
			s += (0xC0 | ((u >> 6) & 0x1F));
			s += (0x80 | (u & 0x3F));
		}
		else if (u <= 0xFFFF) {
			s += (0xE0 | ((u >> 12) & 0x0F));
			s += (0x80 | ((u >> 6) & 0x3F));
			s += (0x80 | (u & 0x3F));
		}
		else {
			s += (0xF0 | ((u >> 18) & 0x07));
			s += (0x80 | ((u >> 12) & 0x3F));
			s += (0x80 | ((u >> 6) & 0x3F));
			s += (0x80 | (u & 0x3F));
		}
		return s;
	}

	void Parser::parse_string_raw(std::string &res)
	{
		expect(p, '"');
		std::string s;
		unsigned u1, u2;
		for (;;) {
			char ch = *p++;
			switch (ch)
			{
			case '"':
				res = s;
				return;
			case '\0':
				throw jsonException("miss quotation mark");
			case '\\':
				switch (*p++)
				{
				case '\"':s += '"'; break;
				case '\\':s += '\\'; break;
				case '/':s += '/'; break;
				case 'b':s += '\b'; break;
				case 'f':s += '\f'; break;
				case 'n':s += '\n'; break;
				case 'r':s += '\r'; break;
				case 't':s += '\t'; break;
				case 'u':
					if (!parse_hex4(u1))
						throw jsonException("invalid unicode hex");
					if (u1 >= 0xD800 && u1 <= 0xDBFF) {
						if (*p++ != '\\')
							throw jsonException("invalid unicode surrogate");
						if (*p++ != 'u')
							throw jsonException("invalid unicode surrogate");
						if (!parse_hex4(u2))
							throw jsonException("invalid unicode hex");
						if (u2 < 0xDC00 || u2>0xDFFF)
							throw jsonException("invalid unicode surrogate");
						u1 = 0x10000 + ((u1 - 0xD800) << 10) + (u2 - 0xDC00);
					}
					s += encode_utf8(u1);
					break;
				default:
					throw jsonException("invalid string escape");
					break;
				}
				break;
			default:
				if (static_cast<unsigned char>(ch) < 0x20)
					throw jsonException("invalid string char");
				s += ch;
				break;
			}
		}
	}

	void Parser::parse_string()
	{
		std::string res;
		try {
			parse_string_raw(res);
		}
		catch (jsonException e) {
			throw;
		}
		v.set_string(res);
	}

	void Parser::parse_array()
	{
		expect(p, '[');
		parse_whitespace();
		std::vector<Value> a;
		if (*p == ']') {
			++p;
			v.set_array(a);
			return;
		}
		for (;;) {
			parse_whitespace();
			try {
				parse_value();
			}
			catch (jsonException e) {
				v.set_literal_type(TYPE_NULL);
				throw;
			}
			a.push_back(v);
			parse_whitespace();
			if (*p == ',')
				++p;
			else if (*p == ']') {
				++p;
				v.set_array(a);
				return;
			}
			else
				throw jsonException("miss comma or square bracket");			
		}
	}

	void Parser::parse_object()
	{
		expect(p, '{');
		parse_whitespace();
		std::vector<Member> o;
		if (*p == '}') {
			v.set_object(o);
			++p;
			return;
		}
		for (;;) {
			Member m;
			std::string res;
			if (*p != '"')
				throw jsonException("miss key");
			try {
				parse_string_raw(res);
			}
			catch (jsonException e) {
				throw;
			}
			m.set_key(res);
			parse_whitespace();
			if (*p != ':') {
				throw jsonException("miss colon");
			}
			++p;
			parse_whitespace();
			try {
				parse_value();
			}
			catch(jsonException e){
				throw;
			}
			m.set_value(this->v);
			o.push_back(m);
			parse_whitespace();
			if (*p == ',') {
				++p;
				parse_whitespace();
			}
			else if (*p == '}') {
				v.set_object(o);
				++p;
				return;
			}
			else
				throw jsonException("miss comma or curly bracket");
		}
	}

	void Parser::parse_value() {
		switch (*p)
		{
		case 'n': parse_literal("null", TYPE_NULL); break;
		case 't': parse_literal("true", TYPE_TRUE); break;
		case 'f': parse_literal("false", TYPE_FALSE); break;
		case '"': parse_string(); break;
		case '[': parse_array(); break;
		case '{': parse_object(); break;
		case '\0': throw jsonException("expect value");
		default: parse_number(); break;
		}
	}



}

