#pragma once

#include <string>
#include <string_view>
#include <vector>

class Tokenizer {
public:
    static std::vector<std::string_view> tokenize(const std::string& text);
};
