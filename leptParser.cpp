#include "leptParser.h"
namespace lept_json {
	inline void expect(const char* &c, char ch) {
		assert(*c == ch);
		++c;
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

	parse_status Parser::parse_null() noexcept {
		expect(p, 'n');
		if (p[0] != 'u' || p[1] != 'l' || p[2] != 'l')
			return PARSE_INVALID_VALUE;
		v.set_type(TYPE_NULL);
		p += 3;
		return PARSE_OK;
	}

	parse_status Parser::parse_true() noexcept	{
		expect(p, 't');
		if (p[0] != 'r' || p[1] != 'u' || p[2] != 'e')
			return PARSE_INVALID_VALUE;
		v.set_type(TYPE_TRUE);
		p += 3;
		return PARSE_OK;
	}

	parse_status Parser::parse_false() noexcept {
		expect(p, 'f');
		if (p[0] != 'a' || p[1] != 'l' || p[2] != 's' || p[3] != 'e')
			return PARSE_INVALID_VALUE;
		v.set_type(TYPE_FALSE);
		p += 4;
		return PARSE_OK;
	}

	parse_status Parser::parse_value() noexcept {
		switch (*p)
		{
		case 'n':return parse_null();
		case 't':return parse_true();
		case 'f':return parse_false();
		case '\0': return PAESE_EXPECT_VALUE;
		default: return PARSE_INVALID_VALUE;
		}
	}

}

