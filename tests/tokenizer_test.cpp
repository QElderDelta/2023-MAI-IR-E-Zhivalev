#include "tokenizer.h"

#include <gtest/gtest.h>

using ResultType = std::vector<std::string_view>;

TEST(Tokenizer, EmptyString) {
    EXPECT_EQ(Tokenizer::tokenize(""), ResultType{});
}

TEST(Tokenizer, AllSpacesString) {
    EXPECT_EQ(Tokenizer::tokenize("      "), ResultType{});
}

TEST(Tokenizer, SingleToken) {
    EXPECT_EQ(Tokenizer::tokenize("abc"), ResultType{"abc"});
}

TEST(Tokenizer, MultipleTokens) {
    EXPECT_EQ(Tokenizer::tokenize("abc zxc qwe"), (ResultType{{"abc", "zxc", "qwe"}}));
}

TEST(Tokenizer, MultipleTokensMultipleSpaces) {
    EXPECT_EQ(Tokenizer::tokenize("     abc    zxc  qwe      "), (ResultType{{"abc", "zxc", "qwe"}}));
}
