#include "lexer.h"

#include <gtest/gtest.h>

TEST(LexerTest, EmptyTerm) {
    EXPECT_EQ(Lexer::processTerm(""), std::string{});
}

TEST(LexerTest, TermWithUpperCase) {
    EXPECT_EQ(Lexer::processTerm("AbCdE"), std::string{"abcde"});
}

TEST(LexerTest, TermWithoutUpperCase) {
    EXPECT_EQ(Lexer::processTerm("abcde"), std::string{"abcde"});
}

TEST(LexerTest, TermWithCommas) {
    EXPECT_EQ(Lexer::processTerm(",abcde,"), std::string{"abcde"});
}

TEST(LexerTest, TermWithSpecialSymbolsInside) {
    EXPECT_EQ(Lexer::processTerm("I'm"), std::string{});
}
