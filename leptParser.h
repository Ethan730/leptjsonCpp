#ifndef LEPTPARSER_H
#define LEPTPARSER_H
#include "leptValue.h"

namespace lept_json {
	class Parser final {
	public:
		Parser(Value& v,const std::string& json);
	private:
		void parse_whitespace() noexcept;
		void parse_literal(const std::string& literal, value_type type);
		void parse_number();

		bool parse_hex4(unsigned&) noexcept;
		std::string encode_utf8(unsigned) noexcept;
		void parse_string();
		void parse_array();
		void parse_value();
		
		const char *p;
		Value &v;
	};
}
#endif // !LEPTPARSER_H

