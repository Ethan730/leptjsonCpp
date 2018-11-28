#include "leptValue.h"
#include "leptParser.h"
#include "gtest\gtest.h"
#include <iostream>
#include "leptException.h"
using namespace jsonCpp;

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

static void test_parse_object() {
	Value v;
	EXPECT_NO_THROW(v.parse(" { } "));
	EXPECT_EQ(TYPE_OBJECT, v.get_type());
	EXPECT_EQ(0, v.get_object_size());

	EXPECT_NO_THROW(v.parse(
		"{ "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
        " } "
	));
	EXPECT_EQ(TYPE_OBJECT, v.get_type());
	EXPECT_EQ(7, v.get_object_size());

	EXPECT_EQ("n", v.get_object_key(0));
	EXPECT_EQ(TYPE_NULL, v.get_object_value(0).get_type());

	EXPECT_EQ("f", v.get_object_key(1));
	EXPECT_EQ(TYPE_FALSE, v.get_object_value(1).get_type());
	
	EXPECT_EQ("t", v.get_object_key(2));
	EXPECT_EQ(TYPE_TRUE, v.get_object_value(2).get_type());

	EXPECT_EQ("i", v.get_object_key(3));
	EXPECT_EQ(TYPE_NUMBER, v.get_object_value(3).get_type());
	EXPECT_DOUBLE_EQ(123.0, v.get_object_value(3).get_number());
	
	EXPECT_EQ("s", v.get_object_key(4));
	EXPECT_EQ(TYPE_STRING, v.get_object_value(4).get_type());
	EXPECT_EQ("abc", v.get_object_value(4).get_string());

	EXPECT_EQ("a", v.get_object_key(5));
	EXPECT_EQ(TYPE_ARRAY, v.get_object_value(5).get_type());
	EXPECT_EQ(3, v.get_object_value(5).get_array_size());
	for (std::size_t i = 0; i < 3; ++i) {
		EXPECT_EQ(TYPE_NUMBER, v.get_object_value(5).get_array_element(i).get_type());
		EXPECT_EQ(i + 1.0, v.get_object_value(5).get_array_element(i).get_number());
	}

	EXPECT_EQ("o", v.get_object_key(6));
	EXPECT_EQ(TYPE_OBJECT, v.get_object_value(6).get_type());
	Value tmp = v.get_object_value(6);
	EXPECT_EQ(TYPE_OBJECT, tmp.get_type());
	for (std::size_t i = 0; i < 3; ++i) {
		EXPECT_EQ(std::string(1, static_cast<char>(i + '1')), tmp.get_object_key(i));
		EXPECT_EQ(TYPE_NUMBER, tmp.get_object_value(i).get_type());
		EXPECT_EQ(i + 1.0, tmp.get_object_value(i).get_number());
	}
}

TEST(jsonCpp,parse_success) {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_string();
	test_parse_array();
	test_parse_object();
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

static void test_parse_miss_key() {
	std::string errMsg = "miss key";
	TEST_PARSE_ERROR(errMsg, "{:1,");
	TEST_PARSE_ERROR(errMsg, "{1:1,");
	TEST_PARSE_ERROR(errMsg, "{true:1,");
	TEST_PARSE_ERROR(errMsg, "{false:1,");
	TEST_PARSE_ERROR(errMsg, "{null:1,");
	TEST_PARSE_ERROR(errMsg, "{[]:1,");
	TEST_PARSE_ERROR(errMsg, "{{}:1,");
	TEST_PARSE_ERROR(errMsg, "{\"a\":1,");
}

static void test_parse_miss_colon() {
	std::string errMsg = "miss colon";
	TEST_PARSE_ERROR(errMsg, "{\"a\"}");
	TEST_PARSE_ERROR(errMsg, "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket() {
	std::string errMsg = "miss comma or curly bracket";
	TEST_PARSE_ERROR(errMsg, "{\"a\":1");
	TEST_PARSE_ERROR(errMsg, "{\"a\":1]");
	TEST_PARSE_ERROR(errMsg, "{\"a\":1 \"b\"");
	TEST_PARSE_ERROR(errMsg, "{\"a\":{}");
}

TEST(jsonCpp, parse_error) {
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
	test_parse_miss_key();
	test_parse_miss_colon();
	test_parse_miss_comma_or_curly_bracket();
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

TEST(jsonCpp, copy) {
	test_copy();
}

#define TEST_ROUNDTRIP(json)\
	do{\
		Value v;\
		EXPECT_NO_THROW(v.parse(json));\
		std::string stringifyRes=v.stringify();\
		EXPECT_EQ(stringifyRes,json);\
	} while(0)

static void test_stringify_number() {
	TEST_ROUNDTRIP("0");
	TEST_ROUNDTRIP("-0");
	TEST_ROUNDTRIP("1");
	TEST_ROUNDTRIP("-1");
	TEST_ROUNDTRIP("1.5");
	TEST_ROUNDTRIP("-1.5");
	TEST_ROUNDTRIP("3.25");
	TEST_ROUNDTRIP("1e+20");
	TEST_ROUNDTRIP("1.234e+20");
	TEST_ROUNDTRIP("1.234e-20");

	TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
	TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
	TEST_ROUNDTRIP("-4.9406564584124654e-324");
	TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
	TEST_ROUNDTRIP("-2.2250738585072009e-308");
	TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
	TEST_ROUNDTRIP("-2.2250738585072014e-308");
	TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
	TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void test_stringify_string() {
	TEST_ROUNDTRIP("\"\"");
	TEST_ROUNDTRIP("\"Hello\"");
	TEST_ROUNDTRIP("\"Hello\\nWorld\"");
	TEST_ROUNDTRIP("\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
	TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void test_stringify_array() {
	TEST_ROUNDTRIP("[]");
	TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void test_stringify_object() {
	TEST_ROUNDTRIP("{}");
	TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}


#define TEST_PARSE_STRINGIFY_PARSE(json)\
	do{\
		Value v;\
		v.parse(json);\
		std::string stringifyRes = v.stringify(); \
		Value v2;\
		v2.parse(stringifyRes);\
		EXPECT_EQ(v, v2);\
	}while(0)

static void test_parse_and_stringify_and_parse() {
	TEST_PARSE_STRINGIFY_PARSE("true");
	TEST_PARSE_STRINGIFY_PARSE("false");
	TEST_PARSE_STRINGIFY_PARSE("123");
	TEST_PARSE_STRINGIFY_PARSE("\"Hello\\nWorld\"");
	TEST_PARSE_STRINGIFY_PARSE("[null,false,true,123,\"abc\",[1,2,3]]");
}

TEST(jsonCpp, roundtrip) {
	TEST_ROUNDTRIP("null");
	TEST_ROUNDTRIP("false");
	TEST_ROUNDTRIP("true");
	test_stringify_number();
	test_stringify_string();
	test_stringify_array();
	test_stringify_object();
	test_parse_and_stringify_and_parse();
}

#define TEST_EQUAL(json1,json2)\
	do{\
		Value v1,v2;\
		v1.parse(json1);\
		v2.parse(json2);\
		EXPECT_EQ(v1,v2);\
	}while(0)

static void test_equal() {
	TEST_EQUAL("true", "true");
	TEST_EQUAL("false", "false");
	TEST_EQUAL("null", "null");
	TEST_EQUAL("123", "123");
	TEST_EQUAL("\"abc\"", "\"abc\"");
	TEST_EQUAL("[]", "[]");
	TEST_EQUAL("[1,2,3]", "[1,2,3]");
	TEST_EQUAL("[[]]", "[[]]");
	TEST_EQUAL("{}", "{}");
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}");
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}");
	TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}");
}

#define TEST_NOT_EQUAL(json1,json2)\
	do{\
		Value v1,v2;\
		v1.parse(json1);\
		v2.parse(json2);\
		EXPECT_NE(v1,v2);\
	}while(0)

static void test_not_equal() {
	TEST_NOT_EQUAL("true", "false");
	TEST_NOT_EQUAL("null", "0");
	TEST_NOT_EQUAL("123", "456");
	TEST_NOT_EQUAL("\"abc\"", "\"abcd\"");
	TEST_NOT_EQUAL("[]", "null");
	TEST_NOT_EQUAL("[1,2,3]", "[1,2,3,4]");
	TEST_NOT_EQUAL("{}", "null");
	TEST_NOT_EQUAL("{}", "[]");
	TEST_NOT_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}");
	TEST_NOT_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}");
	TEST_NOT_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}");

}

TEST(jsonCpp, equal) {
	test_equal();
	test_not_equal();
}

TEST(jsonCpp, swap) {
	Value v1, v2;
	v1.parse("true");
	v2.parse("123");
	std::swap(v1, v2);
	EXPECT_EQ(TYPE_NUMBER, v1.get_type());
	EXPECT_EQ(TYPE_TRUE, v2.get_type());
	EXPECT_DOUBLE_EQ(123.0, v1.get_number());
}
int main(int args, char** argv) {


	testing::InitGoogleTest(&args, argv);
	return RUN_ALL_TESTS();
}