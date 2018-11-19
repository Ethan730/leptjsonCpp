#ifndef LEPTVALUE_H
#define LEPTVALUE_H
#include <string>
namespace lept_json {
	enum value_type :int {
		TYPE_NULL,
		TYPE_TRUE,
		TYPE_FALSE,
		TYPE_NUMBER,
		TYPE_STRING,
		TYPE_ARRAY,
		TYPE_OBJECT
	};
	class Value {
	public:
		void parse(const std::string&);
		lept_json::value_type get_type() const noexcept;
		void set_type(lept_json::value_type) noexcept;
		double get_number() noexcept;
		void set_number(double) noexcept;
	private:
		value_type type = lept_json::TYPE_NULL;
		double n;
	};
}
#endif // !LEPTVALUE_H