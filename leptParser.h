#ifndef LEPTPARSER_H
#define LEPTPARSER_H
#include "leptValue.h"

namespace jsonCpp {
	class Parser final {
	public:
		Parser(Value& v,const std::string& json);
	private:
		void parse_whitespace() noexcept;
		void parse_literal(const std::string& literal, value_type type);
		void parse_number();

		bool parse_hex4(unsigned&) noexcept;
		std::string encode_utf8(unsigned) noexcept;
		void parse_string_raw(std::string&);
		void parse_string();
		void parse_array();
		void parse_object();
		void parse_value();
		
		Value &v;
		const char *p;
	};
}
#endif // !LEPTPARSER_H

