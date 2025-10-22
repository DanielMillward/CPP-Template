#include <gtest/gtest.h>
#include <myprinter.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
	print_me();
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.s
	EXPECT_EQ(7 * 6, 42);
}