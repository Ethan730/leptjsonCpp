#include "leptValue.h"
#include "leptParser.h"
#include "gtest\gtest.h"
#include <iostream>
using namespace lept_json;

static void test_parse_null(){
	Value v;
	EXPECT_NO_THROW(v.parse("null"));
	EXPECT_EQ(value_type::TYPE_NULL, v.get_type());
}

static void test_parse_true() {
	Value v;
	EXPECT_NO_THROW(v.parse(" true "));
	EXPECT_EQ(value_type::TYPE_TRUE, v.get_type());
}

static void test_parse_false() {
	Value v;
	EXPECT_NO_THROW(v.parse(" false\n "));
	EXPECT_EQ(value_type::TYPE_FALSE, v.get_type());
}

#define TEST_NUMBER(expect,json)\
	do{\
		Value v;\
		EXPECT_NO_THROW(v.parse(json));\
		EXPECT_EQ(TYPE_NUMBER,v.get_type());\
		EXPECT_DOUBLE_EQ(expect,v.get_number());\
	}while(0)

static void test_parse_number() {
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
	TEST_NUMBER(1.0000000000000002, "1.0000000000000002");
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

#define TEST_STRING(expect,json)\
	do{\
		Value v;\
		EXPECT_NO_THROW(v.parse(json)); \
		EXPECT_EQ(TYPE_STRING, v.get_type()); \
		EXPECT_EQ(expect,v.get_string());\
	}while(0)

static void test_parse_string() {
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
}

TEST(lept_json,parse_success) {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_string();
}

// type being catched must be parse_status other than int
#define TEST_PARSE_ERROR(errCode, json)  \
	do{\
		Value v;\
		EXPECT_ANY_THROW(v.parse(json));\
		try {\
			v.parse(json);\
		}\
		catch (parse_status err) {\
			EXPECT_EQ(errCode, err); \
			EXPECT_EQ(TYPE_NULL,v.get_type());\
		}\
	} while(0)

static void test_parse_expect_value() {
	TEST_PARSE_ERROR(PAESE_EXPECT_VALUE, "");
	TEST_PARSE_ERROR(PAESE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value() {
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "tru");
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE ,"??");

	/*invalid number*/
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "+0");
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "+1");
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "INF");
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "inf");
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "NAN");
	TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "nan");
}

static void test_parse_root_not_singular() {
	TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "null x");
	TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "\ttrue x");
	TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "trued");

	/* invalid number */
	TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
	TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "0x0");
	TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_number_too_big() {
	TEST_PARSE_ERROR(PARSE_NUMBER_TOO_BIG, "1e309");
	TEST_PARSE_ERROR(PARSE_NUMBER_TOO_BIG, "-1e309");
}

static void test_parse_missing_quotation_mark() {
	TEST_PARSE_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"");
	TEST_PARSE_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {
	TEST_PARSE_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
	TEST_PARSE_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
	TEST_PARSE_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
	TEST_PARSE_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
}

static void test_parse_invalid_string_char() {
	TEST_PARSE_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x01\"");
	TEST_PARSE_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
}

TEST(lept_json, parse_error) {
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number_too_big();
	test_parse_missing_quotation_mark();
	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();
}

int main(int args, char** argv) {
	testing::InitGoogleTest(&args, argv);
	return RUN_ALL_TESTS();
}