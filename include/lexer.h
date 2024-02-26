#pragma once

#include <string>
#include <string_view>

class Lexer {
public:
    static std::string processTerm(std::string_view term);
};
