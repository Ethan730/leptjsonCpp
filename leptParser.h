#ifndef LEPTPARSER_H
#define LEPTPARSER_H
#include "leptValue.h"
#include <cassert>
namespace lept_json {
	enum parse_status {
		PARSE_OK,
		PAESE_EXPECT_VALUE,
		PARSE_INVALID_VALUE,
		PARSE_ROOT_NOT_SINGULAR
	};
	class Parser final {
	public:
		Parser(Value& v,const std::string& json);
	private:
		void parse_whitespace() noexcept;
		parse_status parse_null() noexcept;
		parse_status parse_true() noexcept;
		parse_status parse_false() noexcept;
		parse_status parse_value() noexcept;
		const char *p;
		Value &v;
	};
}
#endif // !LEPTPARSER_H

