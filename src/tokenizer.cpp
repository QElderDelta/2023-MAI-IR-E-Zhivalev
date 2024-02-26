#include "tokenizer.h"


std::vector<std::string_view> Tokenizer::tokenize(const std::string &text) {
    std::vector<std::string_view> result;

    auto word_begin = text.find_first_not_of(' ');

    while (word_begin != std::string::npos) {
        auto word_end = text.find(' ', word_begin + 1);

        if (word_end == std::string::npos) {
            result.emplace_back(text.data() + word_begin, text.size() - word_begin);
            break;
        }

        result.emplace_back(text.data() + word_begin, word_end - word_begin);
        word_begin = text.find_first_not_of(' ', word_end + 1);
    }

    return result;
}
