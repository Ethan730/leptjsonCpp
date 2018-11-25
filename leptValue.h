#ifndef LEPTVALUE_H
#define LEPTVALUE_H
#include <string>
#include <vector>
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
		Value();
		~Value();
		Value(const Value&);
		Value& operator=(const Value&);

		void parse(const std::string&);

		lept_json::value_type get_type() const noexcept;
		
		void set_literal_type(lept_json::value_type) noexcept;

		double get_number() const noexcept;
		void set_number(double) noexcept;

		std::string get_string() const noexcept;
		void set_string(const std::string&) noexcept;
				
		std::size_t get_array_size() const noexcept;
		const Value& get_array_element(std::size_t) const noexcept;
		void set_array(const std::vector<Value>&) noexcept;
	private:
		value_type type = lept_json::TYPE_NULL;
		union {
			std::vector<Value> a;
			std::string s;
			double n;
		};
		void unionFree() noexcept;
		
	};
}
#endif // !LEPTVALUE_H