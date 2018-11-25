#include "leptValue.h"
#include "leptParser.h"
#include "gtest\gtest.h"
#include <iostream>
#include "leptException.h"
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
	TEST_STRING(std::string("Hello\0World",11), "\"Hello\\u0000World\"");
	TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
	TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
	TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

static void test_parse_array() {
	Value v;
	EXPECT_NO_THROW(v.parse("[ ]"));
	EXPECT_EQ(TYPE_ARRAY, v.get_type());
	EXPECT_EQ(0, v.get_array_size());

	EXPECT_NO_THROW(v.parse("[ null , false , true , 123 , \"abc\" ]"));
	EXPECT_EQ(TYPE_ARRAY, v.get_type());
	EXPECT_EQ(5, v.get_array_size());
	EXPECT_EQ(TYPE_NULL, v.get_array_element(0).get_type());
	EXPECT_EQ(TYPE_FALSE,v.get_array_element(1).get_type());
	EXPECT_EQ(TYPE_TRUE, v.get_array_element(2).get_type());
	EXPECT_EQ(TYPE_NUMBER, v.get_array_element(3).get_type());
	EXPECT_DOUBLE_EQ(123, v.get_array_element(3).get_number());
	EXPECT_EQ(TYPE_STRING, v.get_array_element(4).get_type());
	EXPECT_EQ("abc", v.get_array_element(4).get_string());

	EXPECT_NO_THROW(v.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
	EXPECT_EQ(TYPE_ARRAY, v.get_type());
	EXPECT_EQ(4, v.get_array_size());
	EXPECT_EQ(TYPE_ARRAY, v.get_array_element(0).get_type());
	EXPECT_EQ(0, v.get_array_element(0).get_array_size());

	EXPECT_EQ(TYPE_ARRAY, v.get_array_element(1).get_type());
	EXPECT_EQ(1, v.get_array_element(1).get_array_size());

	EXPECT_EQ(TYPE_ARRAY, v.get_array_element(2).get_type());
	EXPECT_EQ(2, v.get_array_element(2).get_array_size());
	
	EXPECT_EQ(TYPE_ARRAY, v.get_array_element(3).get_type());
	EXPECT_EQ(3, v.get_array_element(3).get_array_size());
	EXPECT_DOUBLE_EQ(2, v.get_array_element(3).get_array_element(2).get_number());
}

TEST(lept_json,parse_success) {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_string();
	test_parse_array();
}

// type being catched must be parse_status other than int
#define TEST_PARSE_ERROR(errMsg, json)  \
	do{\
		Value v;\
		try {\
			EXPECT_ANY_THROW(v.parse(json)); \
		}\
		catch (jsonException err) {\
			EXPECT_EQ(errMsg, err.what()); \
			EXPECT_EQ(TYPE_NULL,v.get_type());\
		}\
	} while(0)

static void test_parse_expect_value() {
	std::string errMsg = "expect value";
	TEST_PARSE_ERROR(errMsg, "");
	TEST_PARSE_ERROR(errMsg, " ");
}

static void test_parse_invalid_value() {
	std::string errMsg = "invalid value";
	TEST_PARSE_ERROR(errMsg, "tru");
	TEST_PARSE_ERROR(errMsg,"??");

	/*invalid number*/
	TEST_PARSE_ERROR(errMsg, "+0");
	TEST_PARSE_ERROR(errMsg, "+1");
	TEST_PARSE_ERROR(errMsg, ".123"); /* at least one digit before '.' */
	TEST_PARSE_ERROR(errMsg, "1.");   /* at least one digit after '.' */
	TEST_PARSE_ERROR(errMsg, "INF");
	TEST_PARSE_ERROR(errMsg, "inf");
	TEST_PARSE_ERROR(errMsg, "NAN");
	TEST_PARSE_ERROR(errMsg, "nan");

	/*invalid value in array*/
	TEST_PARSE_ERROR(errMsg, "[1,]");
	TEST_PARSE_ERROR(errMsg, "[\"a\", nul]");
}

static void test_parse_root_not_singular() {
	std::string errMsg = "root not singular";
	TEST_PARSE_ERROR(errMsg, "null x");
	TEST_PARSE_ERROR(errMsg, "\ttrue x");
	TEST_PARSE_ERROR(errMsg, "trued");

	/* invalid number */
	TEST_PARSE_ERROR(errMsg, "0123"); /* after zero should be '.' or nothing */
	TEST_PARSE_ERROR(errMsg, "0x0");
	TEST_PARSE_ERROR(errMsg, "0x123");
}

static void test_parse_number_too_big() {
	std::string errMsg = "number is too big";
	TEST_PARSE_ERROR(errMsg, "1e309");
	TEST_PARSE_ERROR(errMsg, "-1e309");
}

static void test_parse_missing_quotation_mark() {
	std::string errMsg = "miss quotation mark";
	TEST_PARSE_ERROR(errMsg, "\"");
	TEST_PARSE_ERROR(errMsg, "\"abc");
}

static void test_parse_invalid_string_escape() {
	std::string errMsg = "invalid string escape";
	TEST_PARSE_ERROR(errMsg, "\"\\v\"");
	TEST_PARSE_ERROR(errMsg, "\"\\'\"");
	TEST_PARSE_ERROR(errMsg, "\"\\0\"");
	TEST_PARSE_ERROR(errMsg, "\"\\x12\"");
}

static void test_parse_invalid_string_char() {
	std::string errMsg = "invalid string char";
	TEST_PARSE_ERROR(errMsg, "\"\x01\"");
	TEST_PARSE_ERROR(errMsg, "\"\x1F\"");
}

static void test_parse_invalid_unicode_hex() {
	std::string errMsg = "invalid unicode hex";
	TEST_PARSE_ERROR(errMsg, "\"\\u\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u0\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u01\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u012\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u/000\"");
	TEST_PARSE_ERROR(errMsg, "\"\\uG000\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u0/00\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u0G00\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u0/00\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u00G0\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u000/\"");
	TEST_PARSE_ERROR(errMsg, "\"\\u000G\"");
}

static void test_parse_invalid_unicode_surrogate() {
	std::string errMsg = "invalid unicode surrogate";
	TEST_PARSE_ERROR(errMsg, "\"\\uD800\"");
	TEST_PARSE_ERROR(errMsg, "\"\\uDBFF\"");
	TEST_PARSE_ERROR(errMsg, "\"\\uD800\\\\\"");
	TEST_PARSE_ERROR(errMsg, "\"\\uD800\\uDBFF\"");
	TEST_PARSE_ERROR(errMsg, "\"\\uD800\\uE000\"");
}

static void test_parse_miss_comma_or_square_bracket() {
	std::string errMsg = "miss comma or square bracket";
	TEST_PARSE_ERROR(errMsg, "[1");
	TEST_PARSE_ERROR(errMsg, "[1}");
	TEST_PARSE_ERROR(errMsg, "[1 2");
	TEST_PARSE_ERROR(errMsg, "[[]");
}

TEST(lept_json, parse_error) {
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number_too_big();
	test_parse_missing_quotation_mark();
	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();
	test_parse_invalid_unicode_hex();
	test_parse_invalid_unicode_surrogate();
	test_parse_miss_comma_or_square_bracket();
}

static void test_copy() {
	Value v;
	v.parse("\"1abcc\"");
	Value tmp(v);
	EXPECT_EQ(TYPE_STRING, tmp.get_type());
	Value tmp2;
	tmp2.parse("[]");
	tmp2 = v;
	EXPECT_EQ(TYPE_STRING, tmp2.get_type());
}

TEST(lept_json, copy) {
	test_copy();
}
int main(int args, char** argv) {
	testing::InitGoogleTest(&args, argv);
	return RUN_ALL_TESTS();
}