#ifndef LEPTPARSER_H
#define LEPTPARSER_H
#include "leptValue.h"

namespace lept_json {
	enum parse_status {
		PARSE_OK,
		PAESE_EXPECT_VALUE,
		PARSE_INVALID_VALUE,
		PARSE_ROOT_NOT_SINGULAR,
		PARSE_NUMBER_TOO_BIG,
		PARSE_MISS_QUOTATION_MARK,
		PARSE_INVALID_STRING_ESCAPE,
		PARSE_INVALID_STRING_CHAR,
		PARSE_INVALID_UNICODE_HEX,
		PARSE_INVALID_UNICODE_SURROGATE
	};
	class Parser final {
	public:
		Parser(Value& v,const std::string& json);
	private:
		void parse_whitespace() noexcept;
		parse_status parse_literal(const std::string& literal, value_type type) noexcept;
		parse_status parse_number() noexcept;

		bool parse_hex4(unsigned&) noexcept;
		std::string encode_utf8(unsigned) noexcept;
		parse_status parse_string() noexcept;
		parse_status parse_value() noexcept;
		const char *p;
		Value &v;
	};
}
#endif // !LEPTPARSER_H

