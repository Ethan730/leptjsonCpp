#include "leptParser.h"
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdlib>
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
		parse_status ret;
		if ((ret = parse_value()) == PARSE_OK) {
			parse_whitespace();
			if (*p != '\0') {
				ret = PARSE_ROOT_NOT_SINGULAR;
				v.set_type(TYPE_NULL);
			}	
		}
		if (ret != PARSE_OK) {
			//throw("Invalid input, error code: " + std::to_string(ret));
			throw ret;
		}
			
	}

	void Parser::parse_whitespace() noexcept {
		while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
			++p;
		}
	}

	parse_status Parser::parse_literal(const std::string& literal, value_type type) noexcept
	{
		expect(p, literal[0]);
		size_t i;
		for(i=0;i<literal.size()-1;++i)
			if (p[i] != literal[i+1])
				return PARSE_INVALID_VALUE;
		v.set_type(type);
		p += literal.size()-1;
		return PARSE_OK;
	}

	parse_status Parser::parse_number() noexcept
	{
		const char* tmp = p;
		if (*tmp == '-')
			++tmp;
		if (*tmp == '0')
			++tmp;
		else {
			if (!isdigit1to9(*tmp))
				return PARSE_INVALID_VALUE;
			while (isdigit(*tmp))
				++tmp;
		}
		if (*tmp == '.') {
			++tmp;
			if (!isdigit(*tmp))
				return PARSE_INVALID_VALUE;
			while (isdigit(*tmp))
				++tmp;
		}
		if (*tmp == 'e' || *tmp == 'E') {
			++tmp;
			if (*tmp == '+' || *tmp == '-')
				++tmp;
			if (!isdigit(*tmp))
				return PARSE_INVALID_VALUE;
			while (isdigit(*tmp))
				++tmp;
		}
		errno = 0;
		double n = strtod(p, nullptr);
		if (errno == ERANGE&&(n == HUGE_VAL || n == -HUGE_VAL))
			return PARSE_NUMBER_TOO_BIG;
		p = tmp;
		v.set_number(n);
		return PARSE_OK;
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

	parse_status Parser::parse_string() noexcept
	{
		expect(p, '"');
		std::string s;
		unsigned u1, u2;
		for (;;) {
			char ch = *p++;
			switch (ch)
			{
			case '"':
				v.set_string(s);
				return PARSE_OK;
			case '\0':
				return PARSE_MISS_QUOTATION_MARK;
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
						return PARSE_INVALID_UNICODE_HEX;
					if (u1>=0xD800&&u1<=0xDBFF) {
						if (*p++ != '\\')
							return PARSE_INVALID_UNICODE_SURROGATE;
						if (*p++ != 'u')
							return PARSE_INVALID_UNICODE_SURROGATE;
						if (!parse_hex4(u2))
							return PARSE_INVALID_UNICODE_HEX;
						if (u2 < 0xDC00 || u2>0xDFFF)
							return PARSE_INVALID_UNICODE_SURROGATE;
						u1 = 0x10000 + ((u1 - 0xD800) << 10) + (u2 - 0xDC00);
					}
					s += encode_utf8(u1);
					break;
				default:
					return PARSE_INVALID_STRING_ESCAPE;
					break;
				}
				break;
			default:
				if (static_cast<unsigned char>(ch) < 0x20)
					return PARSE_INVALID_STRING_CHAR;
				s += ch;
				break;
			}
		}

		
	}

	parse_status Parser::parse_value() noexcept {
		switch (*p)
		{
		case 'n':return parse_literal("null",TYPE_NULL);
		case 't':return parse_literal("true", TYPE_TRUE);
		case 'f':return parse_literal("false", TYPE_FALSE);
		case '"':return parse_string();
		case '\0': return PAESE_EXPECT_VALUE;
		default: return parse_number();
		}
	}

}

