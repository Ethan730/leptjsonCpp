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

	parse_status Parser::parse_string() noexcept
	{
		expect(p, '"');
		std::string s;
		for (;;) {
			char ch = *p++;
			switch (ch)
			{
			case '"':
				v.set_string(s);
				return PARSE_OK;
			case '\0':
				return LEPT_PARSE_MISS_QUOTATION_MARK;
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
				default:
					return LEPT_PARSE_INVALID_STRING_ESCAPE;
					break;
				}
				break;
			default:
				if (static_cast<unsigned char>(ch) < 0x20)
					return LEPT_PARSE_INVALID_STRING_CHAR;
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

