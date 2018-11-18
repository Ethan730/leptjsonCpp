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

TEST(lept_json,parse_literal) {
	test_parse_null();
	test_parse_true();
	test_parse_false();
}

// type being catched must be parse_status other than int
#define TEST_PARSE_ERROR(json,errCode)  \
	do{\
		Value v;\
		EXPECT_ANY_THROW(v.parse(json));\
		try {\
			v.parse(json);\
		}\
		catch (parse_status err) {\
			EXPECT_EQ(errCode, err); \
		}\
	} while(0)

static void test_parse_expect_value() {
	TEST_PARSE_ERROR("", PAESE_EXPECT_VALUE);
	TEST_PARSE_ERROR(" ", PAESE_EXPECT_VALUE);
}

static void test_parse_invalid_value() {
	TEST_PARSE_ERROR("tru", PARSE_INVALID_VALUE);
	TEST_PARSE_ERROR("??", PARSE_INVALID_VALUE);
}

static void test_parse_root_not_singular() {
	TEST_PARSE_ERROR("null x", PARSE_ROOT_NOT_SINGULAR);
	TEST_PARSE_ERROR("\ttrue x", PARSE_ROOT_NOT_SINGULAR);
	TEST_PARSE_ERROR("trued", PARSE_ROOT_NOT_SINGULAR);
}

TEST(lept_json, parse_error) {
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
}

int main(int args, char** argv) {
	testing::InitGoogleTest(&args, argv);
	return RUN_ALL_TESTS();
}