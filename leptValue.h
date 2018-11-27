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

	class Member;
	class Value {
	public:
		Value();
		~Value();
		Value(const Value&);
		Value& operator=(const Value&);

		void parse(const std::string&);
	private:
		std::string stringify_string(const std::string& s) const noexcept;
		std::string stringify_value() const noexcept;
	public:
		std::string stringify() const noexcept;

		lept_json::value_type get_type() const noexcept;
		
		void set_literal_type(lept_json::value_type) noexcept;

		double get_number() const noexcept;
		void set_number(double) noexcept;

		std::string get_string() const noexcept;
		void set_string(const std::string&) noexcept;
				
		std::size_t get_array_size() const noexcept;
		const Value& get_array_element(std::size_t) const noexcept;
		void set_array(const std::vector<Value>&) noexcept;

		std::size_t get_object_size() const noexcept;
		std::string get_object_key(std::size_t) const noexcept;
		const Value& get_object_value(std::size_t) const noexcept;
		void set_object(const std::vector<Member>&) noexcept;
	private:
		value_type type = lept_json::TYPE_NULL;
		union {
			std::vector<Member> o;
			std::vector<Value> a;
			std::string s;
			double n;
		};
		void unionFree() noexcept;
		
	};

	class Member {
	public:
		std::string get_key() const noexcept;
		void set_key(const std::string&) noexcept;

		const Value& get_value() const noexcept;
		void set_value(const Value&) noexcept;
	private:
		std::string key;
		Value v;
	};
}
#endif // !LEPTVALUE_H