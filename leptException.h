#ifndef LEPTEXCEPTION_H
#define LEPTEXCEPTION_H
#include <string>
#include <stdexcept>
namespace lept_json{
	class jsonException final:public std::logic_error {
	public:
		jsonException(const std::string& err):std::logic_error(err){}
	};
}
#endif // !LEPTEXCEPTION_H

