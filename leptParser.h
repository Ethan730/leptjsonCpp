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
		PARSE_NUMBER_WRONG
	};
	class Parser final {
	public:
		Parser(Value& v,const std::string& json);
	private:
		void parse_whitespace() noexcept;
		parse_status parse_literal(const std::string& literal, value_type type) noexcept;
		parse_status parse_number() noexcept;
		parse_status parse_value() noexcept;
		const char *p;
		Value &v;
	};
}
#endif // !LEPTPARSER_H

